/* This file was generated by ODB, object-relational mapping (ORM)
 * compiler for C++.
 */

DROP TABLE IF EXISTS `Lesson`;

CREATE TABLE `Lesson` (
  `id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
  `title` TEXT NOT NULL,
  `description` TEXT NOT NULL,
  `teacher` TEXT NOT NULL,
  `location` TEXT NOT NULL,
  `startdate` TEXT NOT NULL,
  `enddate` TEXT NOT NULL,
  `year` TEXT NOT NULL,
  `type` TEXT NOT NULL,
  `weekNr` INT NOT NULL)
 ENGINE=InnoDB;

