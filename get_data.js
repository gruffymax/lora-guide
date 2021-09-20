// A Node.js script to subscribe to a TTN MQTT server.

const mqtt = require('mqtt');
const mysql = require('mysql');
const moment = require('moment-timezone');

// Configure our MYSQL database user
const MYSQLoptions={
    host: 'localhost',
    user: 'username',
    password: 'password',
    database: 'your_db_name'
};

// Configure our TTN connection details
const TTNoptions={
  username:"your-app-name@ttn",
  password:"Your-App-Key",
  clean:true
};

//Connect to the MQTT Server
var client  = mqtt.connect('mqtt://eu1.cloud.thethings.network:1883', TTNoptions);

//Listen for a CONNACK signal. This tells us that the connection was
//successfull
client.on("connect", function() {
  console.log("MQTT Connected: "+client.connected);
});

//Listen for any errors that occur during connection
client.on("error", function() {
  console.log("Can't connect "+error);
  process.exit(1)
});

//Listen for a new message and save in database
client.on('message',function(topic, message, packet){
  var data = JSON.parse(message);
  var timestamp = moment.tz(data.time, "Europe/London");
  var timestamputc = timestamp.utc();
  const query = "INSERT INTO sensor_data SET ?";
  const values = {
    time: timestamputc.format("YYYY-MM-DD HH:mm:ss"),
    temperature: data.uplink_message.decoded_payload.temperature / 10
  };

  const con = mysql.createConnection(MYSQLoptions);

  con.connect(function(err) {
    if (err) throw err;
  });

  con.query(query, values, function (err, results) {
    if(err) throw err;
    console.log("Record inserted: ",results.insertId);
  });

  con.end(function(err) {
    if(err) throw err;
  });
});

//Subscribe to the TTN application topic. This is for TTN V3
var topic="v3/your-app@ttn/devices/your-node/up";
client.subscribe(topic,{qos:1});


