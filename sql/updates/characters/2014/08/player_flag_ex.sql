ALTER TABLE `characters`     ADD COLUMN `playerFlagsEx` INT(10) UNSIGNED DEFAULT '0' NOT NULL AFTER `playerFlags`,    CHANGE `playerFlags` `playerFlags` INT(10) UNSIGNED DEFAULT '0' NOT NULL;