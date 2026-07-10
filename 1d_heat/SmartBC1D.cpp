#include <vector>

class BoundaryCondition1D {
public:
    virtual void apply(std::vector<double> &u, double dx) = 0;
    virtual ~BoundaryCondition1D() = default;
};

class DirichletBC1D : public BoundaryCondition1D {};

class NeumannBC1D : public BoundaryCondition1D {};

class PeriodicBC1D : public BoundaryCondition1D {};

void apply_bc(std::vector<double>& u, double dx, const BoundaryCondition1D& bc) {
    bc.apply(u, dx);
}

int main () {










}