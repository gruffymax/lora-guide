# LoRa Guide example files
The two example files included are an Arduino script and a Node.JS script.

## Arduino Script
The Arduino script will require the OTAA credentials updating to your own.
These are setup on TTN or CHirpstack.

## Node.JS Script
An instance of MYSQL will need to be configured for this script to work. Please
create a database and edit the name in the script. 

     CREATE DATABASE node_data;

Create a table with the columns 'ID, time & temperature'.

     CREATE TABLE sensor_data( id INT NOT NULL AUTO_INCREMENT, 
                               time DATETIME, 
                               temperature FLOAT, 
                               PRIMARY KEY (id)
     );
