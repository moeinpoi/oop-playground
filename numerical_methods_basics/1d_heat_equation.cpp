#include <iostream>
#include <memory>
#include <math.h>
#include <functional>

class Grid1D {
public:
    Grid1D(double domLo, double domHi, int numEl) : 
        m_domLo(domLo), 
        m_domHi(domHi),
        m_numEl(numEl), 
        m_numNode(numEl+1),
        x(std::make_unique<double[]>(m_numNode))
        {
            m_elSize = (m_domHi - m_domLo) / m_numEl;
            for (int i = 0; i < m_numNode ; i++) {
                x[i] = i*m_elSize;
            }
        }
    virtual ~Grid1D() {
        std::cout << "Grid1D destructed!" << std::endl;
    }
    void printGrid () {
        for (int i = 0; i < m_numNode; i++) {
            std::cout << "Node: " << i << ", x= " << x[i] << std::endl;
        }
    }

public:
    int m_numEl, m_numNode;
    double m_domLo, m_domHi;
    double m_elSize;
    std::unique_ptr<double[]> x;

};

/*--------------*/

class Field1D {
public:
    Field1D(Grid1D* grid) : 
    m_grid(grid), 
    m_f(std::make_unique<double[]>(m_grid->m_numNode)),
    m_df_dx(std::make_unique<double[]>(m_grid->m_numNode)),
    m_d2f_dx2(std::make_unique<double[]>(m_grid->m_numNode)) {
    }

public:
    Grid1D* m_grid;
    std::unique_ptr<double[]> m_f; //field var
    std::unique_ptr<double[]> m_df_dx; //field var pos. derivative
    std::unique_ptr<double[]> m_d2f_dx2; //field var laplacian
};

/*--------------*/

struct BoundaryCondition1D {
    virtual void apply(Field1D* field, double valLow, double valHi) const = 0;
};

/*--------------*/

struct DirichletBC1D : BoundaryCondition1D {
    void apply(Field1D* field, double valLow, double valHi) const {
        field->m_f[0] = valLow;
        field->m_f[(field->m_grid->m_numNode)-1] = valHi;
    }
};

/*--------------*/

struct NeumannBC1D : BoundaryCondition1D {
    void apply(Field1D* field, double valLow, double valHi) const {
        field->m_df_dx[0] = valLow;
        field->m_df_dx[(field->m_grid->m_numNode)-1] = valHi;
    }
};

/*--------------*/

struct Laplacian1D {
    void apply(Field1D* field) {
        double h = field->m_grid->m_elSize;
        for (int node = 1; node < (field->m_grid->m_numNode)-1; node++ ) {
            field->m_d2f_dx2[node] = (field->m_f[node+1] + field->m_f[node-1] - 2*field->m_f[node]) /  h*h;
        }
    }
};

/*--------------*/

struct TimeIntegrator {
    TimeIntegrator(double dt, double totTime) : 
        m_dt(dt),
        m_totTime(totTime),
        m_step(0),
        m_nextStep(1) {
            m_nSteps = (m_totTime / m_dt) + 1;
        }
    
    double m_totTime;
    double m_dt;
    int m_step, m_nextStep;
    int m_nSteps; //total number of steps
};

/*--------------*/

class HeatProblem1D {
public:
    HeatProblem1D(double alpha, std::function<void(Field1D*)> initCond) :
        m_alpha(alpha),
        m_initCond(initCond) {}

    double operator()(Field1D* field, TimeIntegrator* timeIntegrator, Laplacian1D* laplacian, int node) {
        return m_alpha*(timeIntegrator->m_dt)*(field->m_d2f_dx2[node]) + field->m_f[node];
    }
    
public:
    double m_alpha;
    std::function<void(Field1D*)> m_initCond;
};

void logFieldState(Field1D* field, int step) {
    for (int node = 0; node < field->m_grid->m_numNode ; node++) {
        std::cout << "Node: " << node << ", x= " << field->m_grid->x[node] << ",    temp= " << field->m_f[node] << ",   at step " << step << std::endl;
    }
}

/*--------------*/


struct ExplicitEuler {
    ExplicitEuler(HeatProblem1D* heatProblem, TimeIntegrator* timeIntegrator, Field1D* field, Laplacian1D* laplacian) :
        m_heatProblem(heatProblem),
        m_timeIntegrator(timeIntegrator),
        m_field(field),
        m_laplacian(laplacian) {
            m_fNew = std::make_unique<double[]> (field->m_grid->m_numNode) ;
        }

    void march() {
        for (int step = m_timeIntegrator->m_step ; step < m_timeIntegrator->m_nSteps ; step++) {
            m_laplacian->apply(m_field);
            if (step == 0) {
                for (int node = 1; node < (m_field->m_grid->m_numNode)-1; node++) {
                    m_heatProblem->m_initCond(m_field);
                }
                logFieldState(m_field, step);
            }
            else for (int node = 1; node < (m_field->m_grid->m_numNode)-1; node++) {
                m_fNew[node] = m_heatProblem->operator()(m_field, m_timeIntegrator, m_laplacian, node);
                m_field->m_f[node] = m_fNew[node];
            }
            if(step == 10 || step == 20 || step == 40) logFieldState(m_field, step);
        }
    }

    HeatProblem1D* m_heatProblem;
    TimeIntegrator* m_timeIntegrator;
    Field1D* m_field;
    Laplacian1D* m_laplacian;
    std::unique_ptr<double[]> m_fNew;

};


/*--------------*/
/*--------------*/
/*--------------*/


int main() {

    
    double domLo = 0;
    double domHi = 1;
    int numEl = 20;


    Grid1D heatGrid(domLo, domHi, numEl);
    Field1D heatField(&heatGrid);

    double dt = 0.005;
    double totTime = 0.2;
    TimeIntegrator timeIntegrator(dt, totTime);


    double alpha = 0.1;
    std::function<void(Field1D*)> initCond = [](Field1D* field) { 
        for (int node = 1; node < (field->m_grid->m_numNode)-1 ; node++) {
            field->m_f[node] = sin(M_PI*(field->m_grid->x[node]));
        }
    };

    HeatProblem1D heat_problem(alpha, initCond);
    Laplacian1D laplacian;

    ExplicitEuler fix(&heat_problem, &timeIntegrator, &heatField, &laplacian);
    fix.march();

    
     
    std::cout << "Program ended!" << std::endl;
}