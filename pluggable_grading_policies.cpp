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
    virtual double getBaseAve() const {
        double sum = 0;
        for(double grade : m_grades) {
            sum += grade;
        }
        return m_grades.empty() ? 0 : sum / m_grades.size();
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
    std::string getRole() const override { return "Undergraduate";}
};


class GradStudent : public Student{
public:
    GradStudent(const std::string& name, double thesisScore) : Student(name) , m_thesisScore(thesisScore) {};
    double getAve() const override {
        double base = getBaseAve();
        return (base*0.7) + (m_thesisScore*0.3);
    }
    std::string getRole() const override { return "Graduate";}

public:
    double m_thesisScore;
};


class ExchangeStudent : public Student{
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
    std::string getRole() const override { return "Exchange";}

public:
    int m_topScoreCount;
};


class Course {
public:
    Course(GradingPolicy* gradingPolicy) : m_gradingPolicy(gradingPolicy) {m_students = {};}
    Course(Student* student, GradingPolicy* gradingPolicy) : m_gradingPolicy(gradingPolicy) {
        m_students.push_back(student);}
    Course(std::vector<Student*> students, GradingPolicy* gradingPolicy) : m_gradingPolicy(gradingPolicy) {
        for (auto student : students) {
            m_students.push_back(student);
        }
    }

    void switch_policy(GradingPolicy* gradingPolicy) {
        m_gradingPolicy = gradingPolicy;
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

public:
    std::vector<Student*> m_students;
    GradingPolicy* m_gradingPolicy;
};


class GradingPolicy {
public:
    virtual ~GradingPolicy() = default;
    virtual double get_course_ave() = 0;
};


class StraightAverage : public GradingPolicy , public Course {
public:
    virtual ~StraightAverage() = default;
    double get_course_ave() override {
        double courseTot = 0;
        for (auto student : m_students) {
            courseTot += student->getAve();
        }
        return m_students.empty() ? 0 : courseTot / m_students.size();
    }
};


class IgnoreEmpty : public GradingPolicy , public Course {
public:
    virtual ~IgnoreEmpty() = default;
    double get_course_ave() override {
        double courseTot = 0;
        int count = 0;
        for (auto student : m_students) {
            if (student->m_grades.size() != 0) { 
                courseTot += student->getAve();
                count++;
        }
        return m_students.empty() ? 0 : courseTot / count;
    }
};


class DropLowestK : public GradingPolicy, public Course {
public:
    virtual ~DropLowestK() = default;
    double get_course_ave() override {


    }
    
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

    std::vector<Student*> additionalStudents = {&Foe};

    Course FluidMech(&Moe);
    FluidMech.enroll_student(&Joe);
    FluidMech.enroll_student(additionalStudents);

    for (auto student : FluidMech.m_students)
        student->printInfo();
}