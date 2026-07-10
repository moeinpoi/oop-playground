#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

class Grid2D {
public:
    Grid2D(int nx, int ny, double x_size, double y_size) :
    _nx(nx),
    _ny(ny),
    _x_size(x_size),
    _y_size(y_size) {
        _dx = _x_size/(_nx - 1);
        _dy = _y_size/(_ny - 1);
    }
    int nx() const {return _nx;}
    int ny() const {return _ny;}
    double dx() const {return _dx;}
    double dy() const {return _dy;}
    double x_size() const {return _x_size;}
    double y_size() const {return _y_size;}
private:
    int _nx, _ny;
    double _dx, _dy;
    double _x_size, _y_size;
};

class Field2D {
public:
    Field2D(const Grid2D& grid) :
    _nx(grid.nx()),
    _ny(grid.ny()),
    _dx(grid.dx()),
    _dy(grid.dy()),
    _u(_nx*_ny, 0.0)
    {}
    int nx() const {return _nx;}
    int ny() const {return _ny;}
    double dx() const {return _dx;}
    double dy() const {return _dy;}
    double& operator()(int i, int j) {return _u[(i*_ny + j)];}
    double operator()(int i, int j) const {return _u[(i*_ny + j)];}
private:
    int _nx, _ny;
    double _dx, _dy;
    std::vector<double> _u;
};

class Solver2D {
public:
    Solver2D(Field2D& field, int nsteps, double dt, double alpha) :
    _nsteps(nsteps),
    _dt(dt),
    _alpha(alpha),
    _nx(field.nx()),
    _ny(field.ny())
    {
        _dx = field.dx();
        _r = _alpha*_dt/(_dx*_dx);
    }
    void step(Field2D& field, Field2D& fieldNext) {
        for (int i = 1; i < _nx-1; i++) {
            for (int j = 1; j < _ny-1; j++) {
                (fieldNext)(i,j) = (field)(i,j) + _r*( (field)(i+1,j) + (field)(i-1,j) + (field)(i,j+1) + (field)(i,j-1) - 4*(field)(i,j) );
            }
        }
    }
private:
    int _nx, _ny;
    double _dx, _dy;
    int _nsteps;
    double _dt, _alpha, _r;
};

void apply_Dirichlet_BC(double bc_val, Field2D& field) {
    int i, j;
    j = 0;
    for (i = 0; i < field.nx(); i++) field(i, j) = bc_val;
    j = field.ny()-1;
    for (i = 0; i < field.nx(); i++) field(i, j) = bc_val;
    i = 0;
    for (j = 0; j < field.ny(); j++) field(i, j) = bc_val;
    i = field.nx()-1;
    for (j = 0; j < field.ny(); j++) field(i, j) = bc_val;
}

int main(int argc, char* argv[]) {

     std::filesystem::path out_dir = (argc > 1) ? argv[1] : ".";
     std::filesystem::create_directories(out_dir);
     std::ofstream file(out_dir/"2d_heat_output.csv");


    int grid_num = 20;
    Grid2D temp_grid(grid_num, grid_num, 1, 1);
    Field2D temp(temp_grid);
    for (int i = 0; i < grid_num; i++) {
        for (int j = 0; j < grid_num; j++) {
            temp(i, j) = 10.0;
        }
    }
    int nsteps = 1000;
    double dt = 0.0006;
    double alpha = 1.0;
    Solver2D solver(temp, nsteps, dt, alpha);
    Field2D tempNext(temp_grid);
    int j = grid_num/2;
    for (int timestep = 0; timestep < nsteps; timestep++) {
        apply_Dirichlet_BC(0.0, temp);
        for(int i = 0; i < temp.nx(); i++) {
            if (i > 0) file << ',';
            file << temp(i, j);
        }
        file << std::endl;
        solver.step(temp, tempNext);
        std::swap(temp, tempNext);
    }
}
