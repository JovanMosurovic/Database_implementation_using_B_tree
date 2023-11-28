   # Database implementation using B tree
   
  This project is a C++ implementation of a B-Tree, optimized for managing customer accounts. 
  A B-Tree is a self-balancing tree data structure that maintains sorted data and allows 
  searches, sequential access, insertions, and deletions in logarithmic time. This specific 
  implementation is geared towards managing customer account data, providing efficient 
  operations for insertion, deletion, search, and more.

> The primary objective of this project is to demonstrate the effective application of B-Trees in 
managing large datasets. Throughout the development of this project, I deepened my understanding of the various operations of B-Trees such as node insertion, data retrieval, and efficient balancing techniques, thereby enhancing my skills in data organization and access efficiency.
> 
> This project was developed as the [second university assignment](instructions.pdf) for "Algorithms and Data Structures 2", University of Belgrade School of Electrical Engineering majoring Software Engineering. Please refer to the file for detailed assignment instructions.

  ## Table of Contents

  - [Project Overview](#project-overview)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Compilation and Execution](#compilation-and-execution)
    - [Usage](#usage)
  - [Features](#features)
  - [Note](#note)

  ## Project Overview

  ### Features

  - **Efficient Account Management**: Uses a B-Tree for efficient management of customer accounts.
  - **Insert Account**: Adds new customer accounts to the B-Tree.
  - **Search Account**: Searches for an account based on the account ID.
  - **Remove Account**: Deletes an account from the B-Tree.
  - **Remove All Accounts for a Customer**: Deletes all accounts associated with a specific customer ID.
  - **Print Tree**: Visualizes the structure of the B-Tree.
  - **Load Data from File**: Loads customer and account data from a specified file.
  - **Create Output File**: Generates an output file with customer and account details.

  ## Getting Started

  ### Prerequisites

  To compile and run this application, you need a C++ compiler (e.g., GCC) installed on your system.

  ### Compilation and Execution

  1. Ensure you have a C++ compiler installed.
  2. Clone or download the source code repository.
  3. Navigate to the directory containing the source code.
  4. Compile the code using a command such as `g++ -o b_tree main.cpp`.
  5. Run the compiled program using `./b_tree`.

  ### Usage

  Upon execution, the program displays a menu with various options to interact with the B-Tree. 
  Users can select an option by entering the corresponding number.

  ## Note

  Ensure your terminal supports ASCII escape codes for properly formatted and colorized output.
