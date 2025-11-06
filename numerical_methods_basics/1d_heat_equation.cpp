#include <iostream>
#include <memory>
#include <math.h>

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
    std::unique_ptr<double[]> m_df_dt; //field var time derivative

};

/*--------------*/

struct BoundaryCondition1D {
    virtual void apply(Field1D* field, double valLow, double valHi) const = 0;
};

/*--------------*/

struct DirichletBC1D : BoundaryCondition1D {
    void apply(Field1D* field, double valLow, double valHi) const {
        field->m_f[0] = valLow;
        field->m_f[field->m_grid->m_numEl] = valHi;
    }
};

/*--------------*/

struct NeumannBC1D : BoundaryCondition1D {
    void apply(Field1D* field, double valLow, double valHi) const {
        //to be completed
    }
};

/*--------------*/

struct Laplacian1D {
    void apply(Field1D* field) {
        double h = field->m_grid->m_elSize;
        for (int i = 1; i < (field->m_grid->m_numNode)-1 ; i++) {
            field->m_d2f_dx2[i] = (field->m_f[i+1] - field->m_f[i-1] - 2*field->m_f[i]) /  h*h;
        }
    }
};

/*--------------*/

struct TimeIntegrator {
    TimeIntegrator(double dt, double totTime) : 
        m_dt(dt),
        m_totTime(totTime),
        m_timeStep(0),
        m_nextStep(1) {}
    

    double m_totTime;
    double m_dt;
    int m_timeStep, m_nextStep;
};

/*--------------*/


class HeatProblem1D {
public:
    HeatProblem1D(double alpha) :
        m_alpha(alpha) {}

    void solve(TimeIntegrator* timeIntegrator, Field1D* field) {
        auto InitCondition = [](double x) {return sin(x*M_PI);};
        if (timeIntegrator->m_timeStep == 0) {
            for (int i = 0; i < (field->m_grid->m_numNode)-1; i++) {
                InitCondition(field->m_grid->x[i]);
            }
        }
        //log
        if (timeIntegrator->m_timeStep < timeIntegrator->m_totTime) {
            
        }
    }

public:
    double m_alpha;
};



/*--------------*/
/*--------------*/
/*--------------*/


int main() {

    
    double domLo = 0;
    double domHi = 1;
    int numEl = 10;


    Grid1D heatGrid(domLo, domHi, numEl);
    heatGrid.printGrid();
    Field1D heatField(&heatGrid);
    
    
    
    std::cout << "Program ended!" << std::endl;
}