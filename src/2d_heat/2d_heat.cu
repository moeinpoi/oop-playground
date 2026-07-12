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
    int _nx, _ny; //num elements in x, and y
    double _dx, _dy; //element size in x, and y
    double _x_size, _y_size;
};

class Field2D {
public:
    Field2D(const Grid2D& grid) :
    _nx(grid.nx()),
    _ny(grid.ny()),
    _dx(grid.dx()),
    _dy(grid.dy()),
    _u(_nx*_ny, 0.0),
    _d_u(nullptr)
    {
        cudaMalloc(&_d_u, _nx*_ny*sizeof(double));
    }

    ~Field2D() {
        cudaFree(_d_u);
    }

    Field2D(const Field2D& other) :
    _nx(other.nx()),
    _ny(other.ny()),
    _dx(other.dx()),
    _dy(other.dy()),
    _u(other._u),
    _d_u(nullptr)
    {
        cudaMalloc(&_d_u, _nx*_ny*sizeof(double));
        cudaMemcpy(_d_u, other._d_u, _nx*_ny*sizeof(double), cudaMemcpyDeviceToDevice);
    }

    Field2D& operator=(const Field2D& other) {
        if (this == &other) return *this;
        _nx = other.nx();
        _ny = other.ny();
        cudaFree(_d_u);
        _dx = other.dx();
        _dy = other.dy();
        _u = other._u;
        cudaMalloc(&_d_u, _nx*_ny*sizeof(double));
        cudaMemcpy(_d_u, other._d_u, _nx*_ny*sizeof(double), cudaMemcpyDeviceToDevice);

        return *this;
    }

    void toHost() {
        cudaMemcpy(_u.data(), _d_u, _nx*_ny*sizeof(double), cudaMemcpyDeviceToHost);
    }
    void toDevice() {
        cudaMemcpy(_d_u, _u.data(), _nx*_ny*sizeof(double), cudaMemcpyHostToDevice);
    }

    int nx() const {return _nx;}
    int ny() const {return _ny;}
    double dx() const {return _dx;}
    double dy() const {return _dy;}
    double* devicePtr() { return _d_u;}

    double& operator()(int i, int j) {return _u[(i*_ny + j)];}
    double operator()(int i, int j) const {return _u[(i*_ny + j)];}


private:
    int _nx, _ny;
    double _dx, _dy;
    std::vector<double> _u;
    double* _d_u;
};


__global__ void step_kernel(const double* u, double* uNext, int nx, int ny, double r) {

    int j = blockIdx.x*blockDim.x + threadIdx.x;
    int i = blockIdx.y*blockDim.y + threadIdx.y;

    if (i >= 1 && j >= 1 && i < (nx-1) && j < (ny-1)) {
        uNext[i*ny + j] = 
                        u[i*ny + j] + 
                        r* ( 
                        u[(i+1)*ny + j] + 
                        u[(i-1)*ny + j] +
                        u[(i)*ny + j+1] +
                        u[(i)*ny + j-1] -
                        4*u[i*ny + j] );
    }
} 

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

void step_cpu(Field2D& field, Field2D& fieldNext) {
    for (int i = 1; i < _nx-1; i++) {
        for (int j = 1; j < _ny-1; j++) {
            (fieldNext)(i,j) = (field)(i,j) + _r*( (field)(i+1,j) + (field)(i-1,j) + (field)(i,j+1) + (field)(i,j-1) - 4*(field)(i,j) );
        }
    }
}

void step_gpu(Field2D& field, Field2D& fieldNext) {

    const double* u = field.devicePtr();
    double* uNext = fieldNext.devicePtr();

    dim3 block(32, 8); //fixed for now
    int blocks_x= (_ny + block.x - 1) / block.x;
    int blocks_y = (_nx + block.y - 1) / block.y;
    dim3 grid(blocks_x, blocks_y);

    step_kernel<<<grid, block>>>(u, uNext, _nx, _ny, _r);
    
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

    int j = grid_num/2; //plot line
    for (int timestep = 0; timestep < nsteps; timestep++) {


        apply_Dirichlet_BC(0.0, temp);

        for(int i = 0; i < temp.nx(); i++) {
            if (i > 0) file << ',';
            file << temp(i, j);
        }
        file << std::endl;

        temp.toDevice();
        solver.step_gpu(temp, tempNext);
        tempNext.toHost();
        std::swap(temp, tempNext);
    }
        
}
