#include <iostream>


class Function {

public:
    virtual double evaluate (double x) const = 0;
};

/*--------------*/

class Quadratic : public Function {

public:
    double evaluate (double x) const { return x*x;}

};

/*--------------*/

class Cubic : public Function {

public:
    double evaluate (double x) const { return x*x*x;}

};

/*--------------*/

class FiniteDifference {

public:
    FiniteDifference(Function* funct) : m_funct(funct) {};
    double derivate (double x, double h) {
        return ( m_funct->evaluate(x+h) - m_funct->evaluate(x-h) ) / (2*h);
    }

public:
    Function* m_funct;
};

/*--------------*/
/*--------------*/
/*--------------*/


int main() {

    Cubic qubic_derivate;
    FiniteDifference finite_diff(&qubic_derivate);

    double x = 2;
    double h = 0.01;
    std::cout << "Exact derivate= " << 3*x*x << ", Finite Difference approx.= " << finite_diff.derivate(x, h) << std::endl;

}