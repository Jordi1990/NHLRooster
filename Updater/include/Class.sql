/* This file was generated by ODB, object-relational mapping (ORM)
 * compiler for C++.
 */

DROP TABLE IF EXISTS `Class_changeList`;

DROP TABLE IF EXISTS `Class_lessons`;

DROP TABLE IF EXISTS `Class`;

CREATE TABLE `Class` (
  `id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
  `className` TEXT NOT NULL,
  `classIdString` TEXT NOT NULL,
  `departmentId` INT NOT NULL,
  `cpath` TEXT NOT NULL,
  `departmentString` TEXT NOT NULL)
 ENGINE=InnoDB;

CREATE TABLE `Class_lessons` (
  `object_id` BIGINT UNSIGNED NOT NULL,
  `index` BIGINT UNSIGNED NOT NULL,
  `value` BIGINT UNSIGNED NULL,
  CONSTRAINT `Class_lessons_object_id_fk`
    FOREIGN KEY (`object_id`)
    REFERENCES `Class` (`id`)
    ON DELETE CASCADE)
 ENGINE=InnoDB;

CREATE INDEX `object_id_i`
  ON `Class_lessons` (`object_id`);

CREATE INDEX `index_i`
  ON `Class_lessons` (`index`);

CREATE TABLE `Class_changeList` (
  `object_id` BIGINT UNSIGNED NOT NULL,
  `index` BIGINT UNSIGNED NOT NULL,
  `value` BIGINT UNSIGNED NULL,
  CONSTRAINT `Class_changeList_object_id_fk`
    FOREIGN KEY (`object_id`)
    REFERENCES `Class` (`id`)
    ON DELETE CASCADE)
 ENGINE=InnoDB;

CREATE INDEX `object_id_i`
  ON `Class_changeList` (`object_id`);

CREATE INDEX `index_i`
  ON `Class_changeList` (`index`);

/*
ALTER TABLE `Class_lessons`
  ADD CONSTRAINT `Class_lessons_value_fk`
    FOREIGN KEY (`value`)
    REFERENCES `Lesson` (`id`)
*/

/*
ALTER TABLE `Class_changeList`
  ADD CONSTRAINT `Class_changeList_value_fk`
    FOREIGN KEY (`value`)
    REFERENCES `ChangeItem` (`id`)
*/

