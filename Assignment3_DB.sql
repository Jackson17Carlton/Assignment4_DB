CREATE TABLE Clients (
    c_id int UNIQUE NOT NULL PRIMARY KEY,
    c_name char(50) NOT NULL,
    c_city char(50) NOT NULL,
    c_zip int NOT NULL
);

--Trigger blocking delete row from Clients
CREATE TRIGGER clientNoDelete BEFORE DELETE ON Clients
FOR EACH ROW
BEGIN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Cannot delete from this table';
END;
    

CREATE TABLE Agents (
    a_id int UNIQUE NOT NULL PRIMARY KEY,
    a_name char(50) NOT NULL,
    a_city char(50) NOT NULL,
    a_zip int NOT NULL
);

--Trigger blocking delete row from Agents
CREATE TRIGGER agentNoDelete BEFORE DELETE ON Agents
FOR EACH ROW
BEGIN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Cannot delete from this table';
END;

CREATE TABLE Policy (
    policy_id int UNIQUE NOT NULL PRIMARY KEY,
    name char(50) NOT NULL,
    type char(50) NOT NULL,
    commision_percentage int NOT NULL
);

--Trigger blocking delete row from Policy
CREATE TRIGGER policyNoDelete BEFORE DELETE ON Policy
FOR EACH ROW
BEGIN
    SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Cannot delete from this table';
END;

CREATE TABLE Policies_sold (
    purchase_id int UNIQUE NOT NULL PRIMARY KEY,
    agent_id int,
    client_id int,
    policy_id int,
    date_purchased date NOT NULL,
    amount decimal(6,2) NOT NULL,
    FOREIGN KEY (agent_id) REFERENCES Agents(a_id),
    FOREIGN KEY (client_id) REFERENCES Clients(c_id),
    FOREIGN KEY (policy_id) REFERENCES Policy(policy_id)
);

--Insert data into tables
INSERT INTO Clients VALUES (101, 'CHRIS', 'DALLAS', 43214);
INSERT INTO Clients VALUES (102, 'OLIVIA', 'BOSTON', 83125);
INSERT INTO Clients VALUES (103, 'ETHAN', 'FAYETTEVILLE', 72701);
INSERT INTO Clients VALUES (104, 'DANIEL', 'NEWYORK', 53421);
INSERT INTO Clients VALUES (105, 'TAYLOR', 'ROGERS', 78291);
INSERT INTO Clients VALUES (106, 'CLAIRE', 'PHOENIX', 85011);

INSERT INTO Agents VALUES (201, 'ANDREW', 'DALLAS', 43214);
INSERT INTO Agents VALUES (202, 'PHILLIP', 'DALLAS', 85011);
INSERT INTO Agents VALUES (203, 'JERRY', 'BOSTON', 83125);
INSERT INTO Agents VALUES (204, 'BRYAN', 'ROGERS', 78291);
INSERT INTO Agents VALUES (205, 'TOMMY', 'DALLAS', 43214);
INSERT INTO Agents VALUES (206, 'BRANT', 'FAYETTEVILLE', 72701);
INSERT INTO Agents VALUES (207, 'SMITH', 'ROGERS', 78291);

INSERT INTO Policy VALUES (301, 'CIGNAHEALTH', 'DENTAL', 5);
INSERT INTO Policy VALUES (302, 'GOLD', 'LIFE', 8);
INSERT INTO Policy VALUES (303, 'WELLCARE', 'HOME', 10);
INSERT INTO Policy VALUES (304, 'UNITEDHEALTH', 'HEALTH', 7);
INSERT INTO Policy VALUES (305, 'UNITEDCAR', 'VEHICLE', 9);

INSERT INTO Policies_sold VALUES (401, 204, 106, 303, STR_TO_DATE('2020-01-02', '%Y-%m-%d'), 2000.00);
INSERT INTO Policies_sold VALUES (402, 201, 105, 305, STR_TO_DATE('2019-08-11', '%Y-%m-%d'), 1500.00);
INSERT INTO Policies_sold VALUES (403, 203, 106, 301, STR_TO_DATE('2019-09-08', '%Y-%m-%d'), 3000.00);
INSERT INTO Policies_sold VALUES (404, 207, 101, 305, STR_TO_DATE('2019-06-21', '%Y-%m-%d'), 1500.00);
INSERT INTO Policies_sold VALUES (405, 203, 104, 302, STR_TO_DATE('2019-11-14', '%Y-%m-%d'), 4500.00);
INSERT INTO Policies_sold VALUES (406, 207, 105, 305, STR_TO_DATE('2019-12-25', '%Y-%m-%d'), 1500.00);
INSERT INTO Policies_sold VALUES (407, 205, 103, 304, STR_TO_DATE('2020-10-15', '%Y-%m-%d'), 5000.00);
INSERT INTO Policies_sold VALUES (408, 204, 103, 304, STR_TO_DATE('2020-02-15', '%Y-%m-%d'), 5000.00);
INSERT INTO Policies_sold VALUES (409, 203, 103, 304, STR_TO_DATE('2020-01-10', '%Y-%m-%d'), 5000.00);
INSERT INTO Policies_sold VALUES (410, 202, 103, 303, STR_TO_DATE('2020-01-30', '%Y-%m-%d'), 2000.00);

--Show table constraints
SELECT COLUMN_NAME, CONSTRAINT_NAME, REFERENCED_COLUMN_NAME, REFERENCED_TABLE_NAME FROM INFORMATION_SCHEMA.KEY_COLUMN_USAGE 
WHERE REFERENCED_COLUMN_NAME IS NOT NULL AND CONSTRAINT_SCHEMA = 'jlcarlto';

--Question 6
SELECT a_id, a_name, a_city, a_zip
FROM Agents
WHERE a_city = 
    (SELECT c_city
     FROM Clients
     WHERE c_name = 'Taylor');

--Question 7
SELECT name, type, MAX(commision_percentage) AS commision_percentage
FROM Policy p
WHERE 2=(SELECT COUNT(DISTINCT commision_percentage) FROM Policy t
WHERE p.commision_percentage <= t.commision_percentage);

--Question 8
SELECT COUNT(s.client_id) AS numSold, p.name, p.type
FROM Policies_sold s, Policy p
WHERE s.client_id = (SELECT c_id
                     FROM Clients
                     WHERE c_city = 'Fayetteville') 
                     AND s.policy_id = p.policy_id
GROUP BY s.policy_id;

--Question 9
SELECT p.name, p.type, COUNT(YEAR(s.date_purchased)) AS COUNT
FROM Policies_sold s, Policy p
WHERE s.policy_id = p.policy_id
GROUP BY s.policy_id
ORDER BY COUNT DESC
LIMIT 1;

--Question 10
SELECT a.a_name,  commision_percentage/100 * s.amount AS commision_earned_per_sale
FROM Policies_sold s, Policy p, Agents a
WHERE a.a_id = s.agent_id AND s.policy_id = p.policy_id
ORDER BY commision_earned_per_sale DESC;
