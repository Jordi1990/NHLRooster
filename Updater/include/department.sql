/* This file was generated by ODB, object-relational mapping (ORM)
 * compiler for C++.
 */

DROP TABLE IF EXISTS `department`;

CREATE TABLE `department` (
  `id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
  `name` TEXT NOT NULL,
  `stringId` TEXT NOT NULL)
 ENGINE=InnoDB;

