<div align = "center">
<h1>1202 Project Group 9: Library Management System</h1>
</div>

## Project Description
A library management system to give away books in a more efficient and orderly manner during events

### Context
The context behind the problem is that in September 2025, to celebrate the National Library Board (NLB) 30th Anniversary and SG60, 60000 books were given out over a period of 2 days, 13th and 14th September at Bras Basah. The event saw more than 60000 books given out that catered for all ages, from fiction and non-fiction, to mother tongue textbooks. Even though the giveaway was a success (i.e all the books were given out), many reports from both the public and media mentioned that the crowd management was not clear which resulted in unsatisfactory experiences with some people.

### Description
Our program has 2 portions; User and Librarian

#### User
For the User side of the program, users can create membership accounts, allowing them to book timeslots to enter the book giveaway event. This solves the crowding issue. The public can take a maximum of 2 books, and before they can leave the venue, they will have to scan the serial numbers on the books. This is so that it prevents them from taking more than 2 books and taking duplicates of the same book.

#### Librarian
For the Librarian side of the program, the librarian is able to see the number and type of books in any venue or all the venues. The librarian is also able to look at some details of users who have registered as a member. The program features robots that can help to refill books on empty shelves. Librarians call upon robots that are placed in each venue to refill books as needed. The librarian is able to see the statuses of each robot at each venue, be it charging, idle or refilling shelves

Details like user accounts, books left and time slots taken are saved when the program ends, providing a persistent database of books and users

## Built with:
C++

## How to run entire program:
1. Download main.exe and run via Ubuntu Terminal commands on VSCode (./main.exe)
2. Compile it yourself; Download all the header (.h) and source code (.cpp) files. The command should look like:

```sh
g++ -std=c++17 BMS.h QMS.h Robot_Transport_System.h Users.h Venue.h BMS_L.cpp BMS_U.cpp QMS.cpp Robot_Transport_System.cpp User_data.cpp Users.cpp Venue.cpp main.cpp -o .exe  
```
Feel free to edit the executable file to whatever file name you would like your main to be

Remember to download all relevant text files (.txt) and place them within the same directory as main executable file. Code is dependant on the text files

## Programmers:
Oh Chuan Chew

Christopher Lee

Joven Toh

Louis Tan Jun Heng

Tan Ka Heng Anthony

Yeoman Pok Guan Lin

