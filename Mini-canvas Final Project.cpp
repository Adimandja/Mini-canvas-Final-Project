#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <stdexcept>
#include <algorithm>

// Enumeration to define user roles
enum class Role {
    STUDENT,
    INSTRUCTOR,
    ADMINISTRATOR
};

// Base class for all users
class User {
protected:
    int user_id;
    std::string name;
    std::string email;
    Role role;

public:
    User(int id, const std::string& name, const std::string& email, Role role)
        : user_id(id), name(name), email(email), role(role) {}

    virtual void display() const {
        std::cout << "Name: " << name << ", Email: " << email << ", Role: " << roleToString(role) << std::endl;
    }

    std::string getEmail() const {
        return email;
    }

    std::string getName() const {  // Add getName() method to access the name attribute
        return name;
    }

    virtual ~User() {}

protected:
    std::string roleToString(Role role) const {
        switch (role) {
            case Role::STUDENT: return "Student";
            case Role::INSTRUCTOR: return "Instructor";
            case Role::ADMINISTRATOR: return "Administrator";
            default: return "Unknown";
        }
    }
};

// Student class derived from User
class Student : public User {
private:
    std::map<std::string, std::map<std::string, std::string>> grades; // {course_name: {assignment_name: grade}}

public:
    Student(int id, const std::string& name, const std::string& email)
        : User(id, name, email, Role::STUDENT) {}

    void enroll(const std::string& course) {
        grades[course]; // Initialize grades for the new course
    }

    void viewGrades() const {
        for (const auto& course : grades) {
            std::cout << "Grades for " << course.first << ":" << std::endl;
            for (const auto& assignment : course.second) {
                std::cout << "- " << assignment.first << ": " << assignment.second << std::endl;
            }
        }
    }

    void submitAssignment(const std::string& course, const std::string& assignment, const std::string& grade) {
        grades[course][assignment] = grade;
    }

    // Overload the << operator for displaying grades
    friend std::ostream& operator<<(std::ostream& os, const Student& student) {
        student.viewGrades();
        return os;
    }
};

// Instructor class derived from User
class Instructor : public User {
private:
    std::vector<std::string> courses;

public:
    Instructor(int id, const std::string& name, const std::string& email)
        : User(id, name, email, Role::INSTRUCTOR) {}

    void addCourse(const std::string& course) {
        courses.push_back(course);
    }

    void createAssignment(const std::string& course, const std::string& assignmentName) {
        if (std::find(courses.begin(), courses.end(), course) != courses.end()) {
            std::cout << "Assignment '" << assignmentName << "' created for course '" << course << "'." << std::endl;
        } else {
            std::cout << "You are not teaching this course." << std::endl;
        }
    }

    void gradeAssignment(Student& student, const std::string& course, const std::string& assignmentName, const std::string& grade) {
        student.submitAssignment(course, assignmentName, grade);
    }

    void listCourses() const {
        std::cout << "Courses taught by " << name << ":" << std::endl;
        for (const auto& course : courses) {
            std::cout << "- " << course << std::endl;
        }
    }
};

// Administrator class derived from User
class Administrator : public User {
public:
    Administrator(int id, const std::string& name, const std::string& email)
        : User(id, name, email, Role::ADMINISTRATOR) {}
};

// Exception class for handling user errors
class UserException : public std::runtime_error {
public:
    explicit UserException(const std::string& message) : std::runtime_error(message) {}
};

// Main class for managing the LMS
class MiniCanvas {
private:
    std::vector<Student> students;
    std::vector<Instructor> instructors;
    std::vector<Administrator> administrators;
    std::vector<std::string> courses;

public:
    void addStudent(int id, const std::string& name, const std::string& email) {
        students.emplace_back(id, name, email);
    }

    void addInstructor(int id, const std::string& name, const std::string& email) {
        instructors.emplace_back(id, name, email);
    }

    void addAdministrator(int id, const std::string& name, const std::string& email) {
        administrators.emplace_back(id, name, email);
    }

    Student* findStudent(const std::string& email) {
        for (auto& student : students) {
            if (student.getEmail() == email) {
                return &student;
            }
        }
        throw UserException("Student not found.");
    }

    Instructor* findInstructor(const std::string& email) {
        for (auto& instructor : instructors) {
            if (instructor.getEmail() == email) {
                return &instructor;
            }
        }
        throw UserException("Instructor not found.");
    }

    Administrator* findAdministrator(const std::string& email) {
        for (auto& admin : administrators) {
            if (admin.getEmail() == email) {
                return &admin;
            }
        }
        throw UserException("Administrator not found.");
    }

    void enrollStudentInCourse(const std::string& studentEmail, const std::string& courseName) {
        try {
            Student* student = findStudent(studentEmail);
            student->enroll(courseName);
            std::cout << student->getName() << " enrolled in " << courseName << "." << std::endl;
        } catch (const UserException& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void addInstructorToCourse(const std::string& instructorEmail, const std::string& courseName) {
        try {
            Instructor* instructor = findInstructor(instructorEmail);
            instructor->addCourse(courseName);
            std::cout << instructor->getName() << " assigned to " << courseName << "." << std::endl;
        } catch (const UserException& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void displayCourses() const {
        std::cout << "Courses:" << std::endl;
        for (const std::string& course : courses) {
            std::cout << "- " << course << std::endl;
        }
    }

    void createCourse(const std::string& courseName) {
        courses.push_back(courseName);
        std::cout << "Course '" << courseName << "' created." << std::endl;
    }

    void createAssignmentForCourse(const std::string& instructorEmail, const std::string& courseName, const std::string& assignmentName) {
        try {
            Instructor* instructor = findInstructor(instructorEmail);
            instructor->createAssignment(courseName, assignmentName);
        } catch (const UserException& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void gradeStudentAssignment(const std::string& instructorEmail, const std::string& courseName, const std::string& assignmentName, const std::string& studentEmail, const std::string& grade) {
        try {
            Instructor* instructor = findInstructor(instructorEmail);
            Student* student = findStudent(studentEmail);
            instructor->gradeAssignment(*student, courseName, assignmentName, grade);
        } catch (const UserException& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void viewStudentGrades(const std::string& studentEmail) {
        try {
            Student* student = findStudent(studentEmail);
            std::cout << *student;
        } catch (const UserException& e) {
            std::cout << e.what() << std::endl;
        }
    }

    void saveDataToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Failed to open file for writing." << std::endl;
            return;
        }
        for (const auto& student : students) {
            file << "Student: " << student.getName() << ", " << student.getEmail() << std::endl;
        }
        for (const auto& instructor : instructors) {
            file << "Instructor: " << instructor.getName() << ", " << instructor.getEmail() << std::endl;
        }
        for (const auto& admin : administrators) {
            file << "Administrator: " << admin.getName() << ", " << admin.getEmail() << std::endl;
        }
        file.close();
    }

    void loadDataFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Failed to open file for reading." << std::endl;
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
        file.close();
    }
};

int main() {
    MiniCanvas canvas;

    // Add users
    canvas.addStudent(1, "Alice", "alice@student.com");
    canvas.addInstructor(2, "Dr. Smith", "drsmith@instructor.com");
    canvas.addAdministrator(3, "Admin Bob", "admin@admin.com");

    // Create courses through MiniCanvas
    canvas.createCourse("Computer Science 101");
    canvas.createCourse("Math 101");

    // Display courses
    canvas.displayCourses();

    // Enroll student in a course
    canvas.enrollStudentInCourse("alice@student.com", "Computer Science 101");

    // Assign instructor to a course
    canvas.addInstructorToCourse("drsmith@instructor.com", "Computer Science 101");

    // Create assignment for a course
    canvas.createAssignmentForCourse("drsmith@instructor.com", "Computer Science 101", "Assignment 1");

    // Grade student's assignment
    canvas.gradeStudentAssignment("drsmith@instructor.com", "Computer Science 101", "Assignment 1", "alice@student.com", "A+");

    // View student's grades
    canvas.viewStudentGrades("alice@student.com");

    // Save data to file
    canvas.saveDataToFile("lms_data.txt");

    // Load data from file
    canvas.loadDataFromFile("lms_data.txt");

    return 0;
}
