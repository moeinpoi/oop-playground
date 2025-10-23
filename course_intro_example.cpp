#include <iostream>
#include <string>
#include <vector>

class Student{

public:
    Student() = default;
    Student(const std::string& name):
        m_name(name){};

    virtual double getAve() const {
        double sum = 0;
        for(double grade : m_grades) {
            sum += grade;
        }
        return m_grades.empty() ? 0 : sum / m_grades.size();
    }

    std::string getName() const { return m_name;}

public:
    std::string m_name;
    std::vector<double> m_grades;
};


class GradStudent : public Student{

public:
    GradStudent(const std::string& name, double thesisScore) : Student(name) , m_thesisScore(thesisScore) {};
    double getAve() const override {
        double base = Student::getAve();
        return (base*0.7) + (m_thesisScore*0.3);
    }

public:
    double m_thesisScore;
};

class Course{

public:
    Course() {m_students = {};}
    Course(Student* student) {m_students.push_back(student);}
    Course(std::vector<Student*> students) {
        for (auto student : students) {
            m_students.push_back(student);
        }
    }

    void enroll_student(Student* student) {
        m_students.push_back(student);
    }
    void enroll_student(std::vector<Student*> students) {
        for (auto student : students) {
            m_students.push_back(student);
        }
    }

    void printRoster() const {
        for (auto student : m_students) {
            std::cout << "- " << student->getName() << ": Average= " << student->getAve() << std::endl;
        }
    }

    double getCourseAve () const {
        double courseTot = 0;
        for (auto student : m_students) {
            courseTot += student->getAve();
        }
        return m_students.empty() ? 0 : courseTot / m_students.size();
    } 

public:
    std::vector<Student*> m_students;


};
int main () {

    Student Moe("Moe");
    Moe.m_grades.push_back(20);
    GradStudent Joe("Joe", 20);
    Joe.m_grades.push_back(20);


    Student Foe("Foe");
    Foe.m_grades.push_back(20);
    GradStudent Roe("Roe", 17);
    Roe.m_grades.push_back(20);

    std::vector<Student*> additionalStudent = {&Foe, &Roe};

    Course FluidMech(&Moe);
    FluidMech.enroll_student(&Joe);
    FluidMech.enroll_student(additionalStudent);

    FluidMech.printRoster();
    std:: cout << "Class Average: " << FluidMech.getCourseAve() << std::endl;

}