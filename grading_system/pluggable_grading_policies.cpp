#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

class Person {
public: 
    virtual ~Person() = default;    
    virtual void printInfo() const = 0;
};


class Student : public Person {
public:
    Student() = default;
    ~Student() override = default;
    Student(const std::string& name): m_name(name){};
    void printInfo() const override {
        std::cout << getName() << " (" << getRole() << ") - Average: " << getAve() << std::endl; 
    }  
    virtual std::string getRole() const = 0;
    virtual double getAve() const = 0;
    virtual double getAve(std::vector<double> grades) const = 0;
    double getBaseAve() const {
        double sum = 0;
        for(double grade : m_grades) {
            sum += grade;
        }
        return m_grades.empty() ? 0 : sum / m_grades.size();
    }
    double getBaseAve(std::vector<double> grades) const {
        double sum = 0;
        for(double grade : grades) {
            sum += grade;
        }
        return grades.empty() ? 0 : sum / grades.size();
    }
    const std::string& getName() const { return m_name;}

public:
    std::string m_name;
    std::vector<double> m_grades;
};


class UnderGradStudent : public Student{
public:
    UnderGradStudent(const std::string& name) : Student(name) {};
    double getAve() const override {
        return getBaseAve();
    }
    double getAve(std::vector<double> grades) const override {
        return getBaseAve(grades);
    }
    std::string getRole() const override { return "Undergraduate";}
};


class GradStudent : public Student{
public:
    GradStudent(const std::string& name, double thesisScore) : Student(name) , m_thesisScore(thesisScore) {};
    double getAve() const override {
        double base = getBaseAve();
        return (base*0.7) + (m_thesisScore*0.3);
    }
    double getAve(std::vector<double> grades) const override {
        double base = getBaseAve(grades);
        return (base*0.7) + (m_thesisScore*0.3);
    }
    std::string getRole() const override { return "Graduate";}

public:
    double m_thesisScore;
};


class ExchangeStudent : public Student {
public:
    ExchangeStudent(const std::string& name, int topScoreCount) : Student(name) , m_topScoreCount(topScoreCount) {};
    double getAve() const override {
        double sum = 0;
        std::vector<double> grades = m_grades;
        std::sort(grades.begin(), grades.end(), std::greater<double>());
        for (int i = 0; i < m_topScoreCount; i++) {
            sum += grades[i];
        }
        return  !m_topScoreCount ? 0 : sum / m_topScoreCount;
    }
    double getAve(std::vector<double> igrades) const override {
        double sum = 0;
        std::vector<double> grades = igrades;
        std::sort(grades.begin(), grades.end(), std::greater<double>());
        for (int i = 0; i < m_topScoreCount; i++) {
            sum += grades[i];
        }
        return  !m_topScoreCount ? 0 : sum / m_topScoreCount;
    }
    std::string getRole() const override { return "Exchange";}

public:
    int m_topScoreCount;
};


class Course;

class GradingPolicy {
public:
    virtual ~GradingPolicy() = default;
    virtual double get_course_ave() const = 0;
public:
    Course* m_course;
};


class Course {
public:
    Course(GradingPolicy* gradingPolicy) : m_gradingPolicy(gradingPolicy) {
        m_students = {};
        m_gradingPolicy->m_course = this; 
    }
    Course(Student* student, GradingPolicy* gradingPolicy) : m_gradingPolicy(gradingPolicy) {
        m_students.push_back(student);
        m_gradingPolicy->m_course = this; 
    }
    Course(std::vector<Student*> students, GradingPolicy* gradingPolicy) : m_gradingPolicy(gradingPolicy) {
        for (auto student : students) {
            m_students.push_back(student);
            m_gradingPolicy->m_course = this; 
        }
    }

    void switch_policy(GradingPolicy* gradingPolicy) {
        m_gradingPolicy = gradingPolicy;
        m_gradingPolicy->m_course = this; 
    }

    void enroll_student(Student* student) {
        m_students.push_back(student);
    }
    void enroll_student(std::vector<Student*> students) {
        for (auto student : students) {
            m_students.push_back(student);
        }
    }

    void print_roster() const {
        for (auto student : m_students) {
            std::cout << "- " << student->getName() << ": Average= " << student->getAve() << std::endl;
        }
    }

    void print_course_ave() const {
        double ave = m_gradingPolicy->get_course_ave();

        if (ave == -1) std::cout << "There are no eleigible students to calculate class average!" << std::endl;
        else std::cout << "Class average is: " << ave << std::endl;
    }

public:
    std::vector<Student*> m_students;
    GradingPolicy* m_gradingPolicy;
};



class StraightAverage : public GradingPolicy {
public:
    virtual ~StraightAverage() = default;
    double get_course_ave() const override {
        double courseTot = 0;
        for (auto student : m_course->m_students) {
            courseTot += student->getAve();
        }
        return m_course->m_students.empty() ? -1 : courseTot / m_course->m_students.size();
    }
};


class IgnoreEmpty : public GradingPolicy {
public:
    virtual ~IgnoreEmpty() = default;
    double get_course_ave() const override {
        double courseTot = 0;
        int count = 0;
        for (auto student : m_course->m_students) {
            if (student->m_grades.size() != 0) { 
                courseTot += student->getAve();
                count++;
            }
        }
        return m_course->m_students.empty() ? -1 : courseTot / count;
    }
};


class DropLowestK : public GradingPolicy {
public:
    virtual ~DropLowestK() = default;
    DropLowestK(int _k) : m_k(_k) {}
    double get_course_ave() const override {
        double classTot = 0;
        std::vector<double> s_grades;
        int count = 0;
        for (auto student : m_course->m_students) {
            s_grades = student->m_grades;
            std::sort(s_grades.begin(), s_grades.end(), std::greater<double>());
            for (int i = 0; i < m_k; i++) {
                if(!s_grades.empty()) s_grades.pop_back();
            }
            count ++;
            classTot += student->getAve(s_grades);
        }
        return !count ? -1 : classTot / count;
    }

private:
    int m_k;
};



int main () {

    UnderGradStudent Moe("Moe");
    Moe.m_grades.push_back(20);
    Moe.m_grades.push_back(17);
    Moe.m_grades.push_back(12);

    GradStudent Joe("Joe", 20);
    Joe.m_grades.push_back(12);
    Joe.m_grades.push_back(14);

    ExchangeStudent Foe("Foe", 2);
    Foe.m_grades.push_back(20);
    Foe.m_grades.push_back(19);
    Foe.m_grades.push_back(7);
    Foe.m_grades.push_back(12);

    // UnderGradStudent Sam("Sam");

    std::vector<Student*> additionalStudents = {&Foe};

    StraightAverage straight_average;
    IgnoreEmpty ignore_empty;
    DropLowestK drop_lowest(1);

    Course FluidMech(&Moe, &drop_lowest);
    FluidMech.enroll_student(&Joe);
    FluidMech.enroll_student(additionalStudents);

    for (auto student : FluidMech.m_students)  student->printInfo();
    FluidMech.print_course_ave();
    FluidMech.switch_policy(&ignore_empty);
    FluidMech.print_course_ave();
    
}