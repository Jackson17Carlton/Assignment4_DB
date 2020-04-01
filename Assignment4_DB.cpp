// DO:  more ~username/.my.cnf to see your password
// CHANGE:  MYUSERNAME and MYMYSQLPASSWORD to your username and mysql password
// COMPILE:  g++ -Wall -I/usr/include/cppconn -o Assignment4_DB Assignment4_DB.cpp -L/usr/lib -lmysqlcppconn
// RUN:      ./Assignment4_DB
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/metadata.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

sql::Driver *driver;
sql::Connection *con;
sql::Statement *statement;
sql::ResultSet *resultSet;
sql::ResultSetMetaData *metaData;
sql::Connection* Connect(string, string);

void insert(string table, string values );
void initDatabase(const string Username, const string Password, const string SchemaName);
bool query (string q);
void print (sql::ResultSet *resultSet);
void disconnect();
void printRecords(sql::ResultSet *resultSet, int numColumns);
void printHeader(sql::ResultSetMetaData *metaData, int numColumns);
void findAgentsClients();
string * addClient();
void addAgent();
void purchasePolicy(string id, string city);
void cancelPolicy();
void listPolicies();

// Global Variables
int autoClientId = 150; // first client ID then it is increased by one
int autoAgentId = 250; // first Agent ID
int autoPurchaseId = 450;

int main()
{
    string Username = "jlcarlto";             // Change to your own username
    string mysqlPassword = "PhieGh9e";   // Change to your own mysql password
    con = Connect (Username, mysqlPassword); // Starts the connection
    initDatabase(Username, mysqlPassword, Username); // Connects to the specific DB
    int userNum = 0; // Default userNumber
    while(userNum != 6) {
      // Prints the menu
      cout << "What would you like to do?" << endl;
      cout << "The options are " << endl;
      cout << "1) Find all agents and clients in a city" << endl;
      cout << "2) Add a new client, then purchase an available policy" << endl;
      cout << "3) List all policies sold by an agent" << endl;
      cout << "4) Cancel a policy" << endl;
      cout << "5) Add a new agent to a city" << endl;
      cout << "6) Quit" << endl;
      cout << "Option: ";
      cin >> userNum; // user input for what they want to do

      // Executes the menu
      string* details; //pointer to array of client details
      switch(userNum)
      {
        case 1:
          findAgentsClients(); // Function to find both the agents and clients in a given city
          break;
        case 2:
          details = addClient(); //adds client to DB, returns details
          purchasePolicy(details[0], details[1]); //Purchase a policy (details[0] = id, details[1] = city)
          break;
        case 3:
          listPolicies(); // List all policies sold by an agent
          break;
        case 4:
          cancelPolicy(); // Cancels the policy named by the user
          break;
        case 5:
          addAgent(); // Adds an agent to the DB
          break;
        case 6:
          cout << "Quitting the program" << endl; // Tells the user the program is about to end
          disconnect(); // Removes from the user from the DB
          return 0; // ends the program
        default:
          cout << "Please choose a value 1-6" << endl; // default value, error checking
          break;

      }
    }
    disconnect();
}

// Task 1 Finds all of the agents and clients and displays all relevant information
void findAgentsClients()
{
  // Works hardcoded for Dallas atm
    string queryFindAgents = "";
    string queryFindClients = "";
    string response;
    cout << "Which city would you like to query for agents and clients? ";
    cin >> response;
    cout << endl;
    cout << "Showing all agents from " + response;
    cout << endl;

    queryFindAgents = "SELECT A_ID AS Agents_ID, A_NAME AS Agents_name, A_CITY AS Agents_city, A_ZIP as Agents_zip FROM AGENTS WHERE A_CITY = '" + response + "'; ";
    query(queryFindAgents);
    cout << endl;
    cout << "Showing all clients from " + response;
    cout << endl;
    queryFindClients = "SELECT C_ID AS Clients_ID, C_NAME AS Clients_name, C_CITY AS Clients_city, C_ZIP as Clients_zip FROM CLIENTS WHERE C_CITY = '" + response + "'; ";
    query(queryFindClients);
    cout << endl;
}

// Task 2 adds the client to the list
string * addClient()
{
    string firstName, city, insertValues, zipCode, tableName;
    static string clientDetails[2]; //used to pass around client details
    tableName = "CLIENTS";
    cout << "Thanks for buying from us! We just need a little bit of information from you." << endl;
    cout << "What's your first name? ";
    cin >> firstName;
    cin.ignore();
    boost::to_upper(firstName); //Convert first name to upper case
    cout << "What city do you live in? ";
    cin >> city;
    cin.ignore();
    boost::to_upper(city); //Convert city to upper case
    cout << "What is your current zipcode? ";
    cin >> zipCode;
    cin.ignore();
    string clientId = boost::lexical_cast<string>(autoClientId); //convert clientID to string
    insertValues = clientId + ", '" + firstName + "', '" + city + "', " + zipCode;
    //cout << autoClientId << endl;
    cout << "Insert Values: " << insertValues << endl;
    insert(tableName, insertValues); //inserts into Clients table
    clientDetails[0] = clientId;
    clientDetails[1] = city;
    autoClientId++;
    return clientDetails;
}

//Task 2 policy purchasing
void purchasePolicy(string id, string city)
{
    string queryPolicies, policyChoice, printPolicies, clientCity, clientId;
    clientCity = city;
    clientId = id;
    cout << "Enter the type of policy you would like to purchase: ";
    cin >> policyChoice;
    cin.ignore();
    policyChoice = boost::lexical_cast<string>(policyChoice); //convert policyChoice to caps
    queryPolicies = "SELECT TYPE FROM POLICY WHERE TYPE = '" + policyChoice + "';";
    bool typeExists = query(queryPolicies);
    if(typeExists)
    {
        string agentsQuery, policyId, amount, soldQuery, agentId;
        cout << "We do offer " + policyChoice + " insurance policies!" << endl;
        agentsQuery = "SELECT A_ID AS Agents_ID, A_NAME as Agents_name, A_CITY as Your_City FROM AGENTS WHERE A_CITY = '" + clientCity + "';";
        bool agentFound = query(agentsQuery);
        if(agentFound)
        {
            cout << "Enter an agent ID from your town to purchase a policy from: ";
            cin >> agentId;
            printPolicies = "SELECT * FROM POLICY WHERE type = '" + policyChoice + "';";
            query(printPolicies); // Prints table of policies to purchase
            cout << "Enter a policy ID for the insurance policy you'd like to purchase: ";
            cin.ignore();
            cin >> policyId;
            cout << "Enter an amount for the policy (e.g. 2000.00 with two decimal places): ";
            cin.ignore();
            cin >> amount;
            string purchaseId = boost::lexical_cast<string>(autoPurchaseId); //convert purchaseId to string
            soldQuery = purchaseId + ", " + agentId + ", " + clientId + ", " + policyId + ", STR_TO_DATE('2020-03-31', '%Y-%m-%d'), " + amount;
            cout << soldQuery << endl;
            insert("POLICIES_SOLD", soldQuery); //inserts into policies sold
            query("SELECT * FROM POLICIES_SOLD");
            cout << "Your purchase has gone through, reference the policies sold table for proof of purchase!" << endl;
            cout << "Your purchase ID will be " + purchaseId + "." << endl;
            autoPurchaseId++;
        }
        else
        {
          cout << "There were no agents found in the town you provided" << endl;
        }
    }
    else
    {
        cout << "Policy Type does not exist, please try again and choose an existing policy." << endl;
    }
}

//Task 3 Lists all policies sold by a particular agent
void listPolicies()
{
    string agentName, agentCity, agentQuery;
    cout << "Enter the name of the agent whose policy sales you would like to reference: ";
    cin >> agentName;
    cin.ignore();
    cout << "Enter the city of this agent: ";
    cin >> agentCity;
    cin.ignore();
    agentQuery = "SELECT * FROM AGENTS WHERE a_name = '" + agentName + "' AND a_city = '" + agentCity + "';";
    bool agentFound = query(agentQuery);
    if(agentFound)
    {
        string soldQuery, agentId;
        cout << "Enter the agent Id for the agent found in the table: ";
        cin >> agentId;
        cin.ignore();
        cout << "Displaying sales records for " + agentName + ":" << endl;
        soldQuery = "SELECT * FROM POLICIES_SOLD WHERE agent_id = " + agentId + ";";
        bool policiesFound = query(soldQuery);
        if(policiesFound)
        {
          string policyQuery;
          cout << "Displaying types of policies sold by " + agentName + ":" << endl;
          policyQuery = "SELECT name, type, commision_percentage FROM POLICY WHERE policy_id = (SELECT policy_id FROM POLICIES_SOLD WHERE agent_id = " + agentId + ");";
          query(policyQuery);
        }
        else
        {
          cout << "Agent has sold no policies" << endl;
        }
    }
    else
    {
        cout << "Agent was not found" << endl;
    }
}

//task 4 Cancel an existing policy
void cancelPolicy()
{
    string policyId, deleteString, queryCancelPolicy;
    // Query to show all policies
    queryCancelPolicy = "SELECT PURCHASE_ID, AGENT_ID, CLIENT_ID, POLICY_ID, DATE_PURCHASED, AMOUNT FROM POLICIES_SOLD;";
    query(queryCancelPolicy);
    cout << "You are about to cancel a policy" << endl;
    cout << "What is the purchased ID of the policy you would like to cancel? ";
    cin >> policyId;
    // Deletes the policy with the right ID
    deleteString = "DELETE FROM POLICIES_SOLD WHERE purchase_id = " + policyId + ";";
    cout << deleteString << endl;
    //statement = con->createStatement();
    //statement->executeUpdate(deleteString);
    bool policyCanceled = query(deleteString);
    if(!policyCanceled)
    {
        cout << "Policy was not successfully deleted." << endl;
    }
    else
    {
        cout << "Policy successfully deleted." << endl;
    }
}

// Task 5 adds an agent to the list and displays all appropriate agents
void addAgent()
{
  string firstName, city, insertValues, zipCode, tableName, queryFindAgents; // Need to finish adding the query
  tableName = "AGENTS";
  cout << "Thanks for joining the company, We just need a little bit of information from you" << endl;
  cout << "What's your first name? ";
  cin >> firstName;
  cout << endl;
  cout << "What city do you live in? ";
  cin >> city;
  cout << endl;
  cout << "What is your current zipcode? ";
  cin >> zipCode;
  cout << endl;
  // string hardCode = "997" ;
  insertValues = autoAgentId + ", '" + firstName + "', '" + city + "', " + zipCode;
  insert(tableName, insertValues);
  autoAgentId++;
  cout << "Displaying all agents" << endl;
  queryFindAgents = "SELECT A_ID AS Agents_ID, A_NAME AS Agents_name, A_CITY AS Agents_city, A_ZIP as Agents_zip FROM AGENTS;";
  query(queryFindAgents);
  cout << endl;
}

// Connect to the database
sql::Connection* Connect(const string Username, const string Password)
 {
	 try{

		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", Username, Password);
		}

	catch (sql::SQLException &e) {
        cout << "ERROR: SQLException in " << __FILE__;
        cout << " (" << __func__<< ") on line " << __LINE__ << endl;
        cout << "ERROR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << ")" << endl;
        }
   return con;
}

// Disconnect from the database
void disconnect()
{
		delete resultSet;
		delete statement;
		con -> close();
		delete con;
}

// Execute an SQL query passed in as a string parameter
// and print the resulting relation
bool query (string q)
{
        try {
            resultSet = statement->executeQuery(q);
            if(resultSet == "")
            {
              return false;
            }
            cout<<("\n---------------------------------\n");
            cout<<("Query: \n" + q + "\n\nResult: \n");
            print(resultSet);

            cout<<("\n---------------------------------\n");
            return true;
        }
        catch (sql::SQLException e) {

	      cout << "ERROR: SQLException in " << __FILE__;
        cout << " (" << __func__<< ") on line " << __LINE__ << endl;
        cout << "ERROR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << ")" << endl;
        return false;
        }
}

// Print the results of a query with attribute names on the first line
// Followed by the tuples, one per line
void print (sql::ResultSet *resultSet)
{
    try{
		if (resultSet -> rowsCount() != 0)
		{
   		   sql::ResultSetMetaData *metaData = resultSet->getMetaData();
           int numColumns = metaData->getColumnCount();
		   printHeader( metaData, numColumns);
           printRecords( resultSet, numColumns);
		}
        else
			throw runtime_error("ResultSetMetaData FAILURE - no records in the result set");
    }
	catch (std::runtime_error &e) {
    }

}

// Print the attribute names
void printHeader(sql::ResultSetMetaData *metaData, int numColumns)
{
	/*Printing Column names*/
    for (int i = 1; i <= numColumns; i++) {
            if (i > 1)
                cout<<",  ";
            cout<< metaData->getColumnLabel(i); //ColumnName
        }
        cout<<endl;
}

// Print the attribute values for all tuples in the result
void printRecords(sql::ResultSet *resultSet, int numColumns)
{
        while (resultSet->next()) {
            for (int i = 1; i <= numColumns; i++) {
                if (i > 1)
                    cout<<",  ";
                cout<< resultSet->getString(i);
               ;
            }
        cout<<endl;
        }
}

// Insert into any table, any values from data passed in as String parameters
void insert(const string table, const string values)
{
    string query = "INSERT into " + table + " values (" + values + ");";
    statement->executeUpdate(query);
}


// Remove all records and fill them with values for testing
// Assumes that the tables are already created
void initDatabase(const string Username, const string Password, const string SchemaName)
{
        // Create a connection
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", Username, Password);

        // Connect to the MySQL test database
        con->setSchema(SchemaName);

        statement = con->createStatement();
        //statement->executeUpdate("DELETE from FoodOrder");
        //statement->executeUpdate("DELETE from MenuItem");
        //statement->executeUpdate("DELETE from Dish");
        //statement->executeUpdate("DELETE from Restaurant");

        //insert("Restaurant", "0, 'Tasty Thai', 'Asian', 'Dallas'");
        //insert("Restaurant", "3,'Eureka Pizza','Pizza', 'Fayetteville'");
        //insert("Restaurant", "5,'Tasty Thai','Asian', 'Las Vegas'");

}
