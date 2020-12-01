#include <iostream>

using namespace std;


#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <ctime>
#include <array>
#include <chrono>
#include <exception>

class Random
{
public:

    static int rndInt(int left, int right)
    {
       int num = std::rand() % (right - left) + left;
       return num;
    }

    static std::string getRandomName()
    {
        std::string name = std::string("");
        name += (char)rndInt(65, 91);
        int length = rndInt(6, 14);
        for (int i = 1; i < length; i++)
            if (rndInt(0, 31) == 13)
                name += ' ';
            else
                name += (char)rndInt(97, 123);
        return name;
    }

    static std::vector<std::string> getRandomBooksNamesArr(int length)
    {
        std::vector<std::string> nameOfBooks = std::vector<std::string>();
        for (int i = 0; i < length; i++)
            nameOfBooks.push_back(Random::getRandomName());
        return nameOfBooks;
    }
};


class Book
{
private:
    int numberOfCloset;
    int numberOfLine;
    int numberInLine;
    std::string name;

public:

    Book()
    {
        name = std::string("noname_book");
    }

    Book(std::string nm, int cN, int lN, int bN)
    {
        name = nm;
        numberOfCloset = cN;
        numberOfLine = lN;
        numberInLine = bN;
    }

    void getNumberInfo(int nmOfCloset, int nmOfLine, int nmInLine)
    {
        numberInLine = nmInLine;
        numberOfCloset = nmOfCloset;
        numberOfLine = nmOfLine;
    }

    std::string toString()
    {
        return name +
               ", " + std::to_string(numberOfCloset) +
               "." + std::to_string(numberOfLine) +
               "." + std::to_string(numberInLine);
    }
};


class Line
{
private:
    int index;
    std::vector<Book> line;
public:
    Line ()
    {
        index = -1;
        line = std::vector<Book>();
    }

    Line(int i)
    {
        index = i;
        line = std::vector<Book>();
    }

    void AddBook(std::string name, int cN, int lN, int bN)
    {
        Book b = Book(name, cN, lN, bN);
        line.push_back(b);
    }

    std::string toString()
    {
        std::string str = std::string("    Line " + std::to_string(index) + "\n");
        for (auto elem: line)
        {
            str += "        " + elem.toString() + "\n";
        }
        return str;
    }
};


class Closet
{
private:
    int index;
    std::vector<Line> lines;
public:
    int getIndex() { return index; }

    Closet(int i)
    {
        lines = std::vector<Line>();
        index = i;
    }

    void AddLine(int i)
    {
        Line l = Line(i);
        lines.push_back(l);
    }


    void AddBook(int i, std::string name, int cN, int lN, int bN)
    {
        if (lines.size() > i)
            lines[i].AddBook(name, cN, lN, bN);
    }

    std::string toString()
    {
        std::string str = std::string("Closet " + std::to_string(index) + "\n");
        for (auto elem : lines)
        {
            str += elem.toString();
        }
        return str;
    }

    static void sortVector(std::vector<Closet> &vec)
    {
        int length = vec.size();
        for (int i = 0; i < length - 1; i++)
            for (int j = 0; j < length - i - 1; j++)
                if (vec[j].getIndex() > vec[j + 1].getIndex())
                    std::swap(vec[j], vec[j + 1]);
    }
};


class MyThread
{
public:
    static std::vector<std::array<int, 2>> GetPortfile(int nS, int nL, int nB)
    {
    std::vector<std::array<int, 2>> portfile = std::vector<std::array<int, 2>>();
    for (int i = 0; i < nS - 1; i++){
        std::array<int, 2> arr = std::array<int, 2>();
        arr[0] = i * (nL * nB);
        arr[1] = (i + 1) * (nL * nB) - 1;
        portfile.push_back(arr);
        }
    std::array<int, 2> arr = std::array<int, 2>();
    arr[0] = (nS - 1) * nL * nB;
    arr[1] = nS * nL * nB - 1;
    portfile.push_back(arr);
    return portfile;
}


    static void getOneCloset(std::vector<std::array<int, 2>> &portfile,
                             std::vector<Closet>& closens, int lN,
                            int bN, std::vector<std::string> books)
    {
    std::array<int, 2> arr;
    while (!portfile.empty())
    {
        arr = portfile[portfile.size() - 1];
        portfile.pop_back();

        int iC = (int)(arr[1] / lN / bN);
        Closet cl = Closet(iC + 1);
        for (int iL = 0; iL < lN; iL++){
            cl.AddLine(iL + 1);
            for (int iB = 0; iB < bN; iB++){
                cl.AddBook(iL, books[arr[0] + iL * bN + iB], iC + 1, iL + 1, iB + 1);
            }
        }
        closens.push_back(cl);
    }
}
};


int main()
{
    std::srand(std::time(NULL));
    int cN = Random::rndInt(2, 11);
    int lN = Random::rndInt(2, 21);
    int bN = Random::rndInt(2, 31);
    int length = cN * lN * bN;
    std::vector<std::string> books = Random::getRandomBooksNamesArr(length);

    std::cout << "We have " << std::to_string(cN) << " closets, " <<
              std::to_string(lN) << " lines, " << std::to_string(bN) <<
              " books in every line and total " << std::to_string(length) <<
              " books with names.\nAny book will have name and id: S.L.N, number of closet" <<
              ", number of line and number in line.\nTo restore catalog, push ENTER...\n";
    std::cin.get();
    std::vector<std::array<int, 2>> portfile = MyThread::GetPortfile(cN, lN, bN);
    std::vector<Closet> closets = std::vector<Closet>();


#pragma omp parallel sections num_threads(4)
{
    #pragma omp section
    {
        MyThread::getOneCloset(std::ref(portfile), std::ref(closets), lN, bN, books);
    }
}
    Closet::sortVector(closets);
    for (auto elem: closets)
    {
        std::cout << elem.toString() << "\n\n";
    }
    return 0;
}
