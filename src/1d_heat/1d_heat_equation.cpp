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
    static void apply(Field1D* field) {
        double h = field->m_grid->m_elSize;
        for (int node = 1; node < (field->m_grid->m_numNode)-1; node++ ) {
            field->m_d2f_dx2[node] = (field->m_f[node+1] + field->m_f[node-1] - 2*field->m_f[node]) /  h*h;
        }
    }
};

/*--------------*/

class LogFields {
public:
    LogFields(int stride) : 
        m_stride(stride) {}
    //print fields every m_stride steps
    void logFieldState(Field1D* field, int step) {
        if (step % m_stride == 0) {
            for (int node = 0; node < field->m_grid->m_numNode ; node++) {
                std::cout << "Node: " << node << ", x= " << field->m_grid->x[node] << ",    temp= " << field->m_f[node] << ",   at step " << step << std::endl;
            }
        }
    }
private:
    int m_stride; //print every stride timesteps
};

/*--------------*/

class TimeIntegrator {
public:
    TimeIntegrator(double dt, double totTime) : 
        m_dt(dt),
        m_totTime(totTime),
        m_step(0),
        m_nextStep(m_step+1) {
            m_nSteps = (m_totTime / m_dt) + 1;
        }
    virtual void march() = 0;
    
protected:
    friend class HeatProblem1D; //should later be generelied to an overarching Problem class
    double m_totTime;
    double m_dt;
    int m_step, m_nextStep;
    int m_nSteps; //total number of steps
    
};

/*--------------*/

class HeatProblem1D {
public:
    HeatProblem1D(Field1D* field, double alpha, std::function<void(Field1D*)> initCond) :
        m_field(field),
        m_alpha(alpha),
        m_initCond(initCond) {}

    double operator()(TimeIntegrator* timeIntegrator, int node) {
        return m_alpha*(timeIntegrator->m_dt)*(m_field->m_d2f_dx2[node]) + m_field->m_f[node];
    }
    
private:
    double m_alpha;

public:
    Field1D* m_field;
    std::function<void(Field1D*)> m_initCond;
};

/*--------------*/


class ExplicitEuler : public TimeIntegrator {
public:
    ExplicitEuler(HeatProblem1D* heatProblem, LogFields* logger, double dt, double totTime) :
        TimeIntegrator(dt, totTime),
        m_heatProblem(heatProblem),
        m_logger(logger) {
            m_fNew = std::make_unique<double[]> (heatProblem->m_field->m_grid->m_numNode) ;
        }

    void march() {
        for ( ; m_step < m_nSteps ; m_step++) {
            Laplacian1D::apply(m_heatProblem->m_field);
            if (m_step == 0) {
                for (int node = 1; node < (m_heatProblem->m_field->m_grid->m_numNode)-1; node++) {
                    m_heatProblem->m_initCond(m_heatProblem->m_field);
                }
            }
            else for (int node = 1; node < (m_heatProblem->m_field->m_grid->m_numNode)-1; node++) {
                m_fNew[node] = m_heatProblem->operator()(this, node);
                m_heatProblem->m_field->m_f[node] = m_fNew[node];
            }
            m_logger->logFieldState(m_heatProblem->m_field, m_step);
        }
    }

private:
    HeatProblem1D* m_heatProblem;
    LogFields* m_logger; 
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


    double alpha = 0.1;
    //initial condition: f(x) = sin(PI*x)
    std::function<void(Field1D*)> initCond = [](Field1D* field) { 
        for (int node = 1; node < (field->m_grid->m_numNode)-1 ; node++) {
            field->m_f[node] = sin(M_PI*(field->m_grid->x[node]));
        }
    };


    HeatProblem1D heat_problem(&heatField, alpha, initCond);

    int log_every = 5000;
    LogFields logger(log_every);

    double dt = 0.05;
    double totTime = 5000;
    ExplicitEuler fix(&heat_problem, &logger, dt, totTime);
    fix.march();

     
    std::cout << "Program ended!" << std::endl;
}