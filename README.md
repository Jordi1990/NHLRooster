# NHLRooster
The goal of this project is to develop an android application which can view the lesson schedule of students of the NHL Hogeschool.
This project consists of 3 components:
- Android application
- Schedule updater
- Web API

# Android application
The android application is named "RoosterNHL" and has been developed with performance as main goal.
It's designed to be fast and that schedules are always available. Even if there is no network connection.
It's based on google's material design and uses only native components from the (at this moment) latest support library.
The application fetches it's information from the Web API.
Minimal required android: 4.0x
# Schedule updater
The schedule updater is named "NHLRoosterUpdater" and has been developed in C++, this results in a crossplatform compatibility.
It also scales well on multiple cores, making it very fast and efficient. 
The updater is designed to be called from a cron job every hour or so, it will fetch new schedules and compare them to the previous fetch.
If there are changes it will send push messages, notifying the people who have that class schedule active.
One update job should take about 10 minutes.

Arguments:
- rebuild: Rebuilds the database from scratch(Does not send push messages).
- dry-run: Puts the GCM push messages in dry-run mode(Messages will not be sent).

**Important note:**
The schedule updater has some issues because of the implementation of the ORM framework(odb), some database errors will not be handled the correct way. Also the change detection is not 100% trustworthy. The processing of the information is limited because for example it does not link the teacher's lesson to the actual lessons for each class. Resulting in a larger database and a non-linked dataset. Future goal is to rework the updater in C#/Mono.

# Web API
The android app communicates to this API, sending back information in XML format.
- fetchWeek.php
  Fetches the schedule of the user requesting it, based on given deviceId
- registerDevice.php
  Registers the android device id and GCM push ID in our database
- search.php
  Handles the search through classes/teachers requests.

# Future plans
- Rework schedule updater in C# with entity framework and NET Core 1.0(Multiplatform compatibility)
- Clean up communication protocol between database and android app 
- Clean up android app code

# License
GNU GPL v3, read LICENSE.txt
