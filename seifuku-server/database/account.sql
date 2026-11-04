/*M!999999\- enable the sandbox mode */ 

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES latin1 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;
DROP TABLE IF EXISTS `account`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `account` (
  `id` int(4) unsigned zerofill NOT NULL AUTO_INCREMENT,
  `name` char(32) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `sex` int(4) unsigned zerofill NOT NULL,
  `age` int(4) unsigned zerofill NOT NULL,
  `phone` char(16) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `email` char(128) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `address` char(128) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `idnumber` char(32) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  `password` char(16) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  PRIMARY KEY (`id`),
  KEY `ix_name` (`name`)
) ENGINE=MyISAM AUTO_INCREMENT=5 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

LOCK TABLES `account` WRITE;
/*!40000 ALTER TABLE `account` DISABLE KEYS */;
INSERT INTO `account` VALUES
(0001,'izanami',0000,0000,'','','','','izanami'),
(0002,'izanagi',0000,0000,'','','','','izanagi'),
(0003,'sakuya',0000,0000,'','','','','sakuya'),
(0004,'hoori',0000,0000,'','','','','hoori');
/*!40000 ALTER TABLE `account` ENABLE KEYS */;
UNLOCK TABLES;
DROP TABLE IF EXISTS `fee`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `fee` (
  `time_stamp` char(14) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `server_name` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `user_name` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `user_id` int(4) unsigned zerofill NOT NULL DEFAULT 0000,
  `start_time` char(19) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `last_point` char(19) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `end_time` char(19) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `fee_name` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `fee_id` int(4) unsigned zerofill NOT NULL DEFAULT 0000,
  `fee_type` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `point` int(4) unsigned zerofill NOT NULL DEFAULT 0000,
  `end_type` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `ip_addr` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT ''
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

LOCK TABLES `fee` WRITE;
/*!40000 ALTER TABLE `fee` DISABLE KEYS */;
/*!40000 ALTER TABLE `fee` ENABLE KEYS */;
UNLOCK TABLES;
DROP TABLE IF EXISTS `logon`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `logon` (
  `time_stamp` char(14) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `account` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `account_id` int(4) unsigned zerofill NOT NULL DEFAULT 0000,
  `fee_type` int(4) unsigned zerofill NOT NULL DEFAULT 0000,
  `server` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `authen` int(4) unsigned zerofill NOT NULL DEFAULT 0000,
  `client_ip` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `fee_account` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `notify` char(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `rejoin` tinyint(1) unsigned zerofill NOT NULL DEFAULT 0
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

LOCK TABLES `logon` WRITE;
/*!40000 ALTER TABLE `logon` DISABLE KEYS */;
/*!40000 ALTER TABLE `logon` ENABLE KEYS */;
UNLOCK TABLES;
DROP TABLE IF EXISTS `point001`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `point001` (
  `account_id` int(4) unsigned zerofill NOT NULL,
  `type` int(4) unsigned zerofill NOT NULL,
  `point` int(4) unsigned zerofill NOT NULL,
  `pointtime` int(4) unsigned zerofill NOT NULL,
  `online` int(4) unsigned zerofill NOT NULL,
  `licence` int(4) unsigned NOT NULL,
  `netbar_ip` char(15) DEFAULT NULL,
  `ip_mask` char(15) DEFAULT NULL,
  PRIMARY KEY (`account_id`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

LOCK TABLES `point001` WRITE;
/*!40000 ALTER TABLE `point001` DISABLE KEYS */;
INSERT INTO `point001` VALUES
(0001,0001,1000,0000,0000,0,'',''),
(0002,0001,1000,0000,0000,0,'',''),
(0003,0001,1000,0000,0000,0,'',''),
(0004,0001,1000,0000,0000,0,'','');
/*!40000 ALTER TABLE `point001` ENABLE KEYS */;
UNLOCK TABLES;
DROP TABLE IF EXISTS `serial`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `serial` (
  `time_stamp` char(14) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT '',
  `account_id` int(4) unsigned zerofill NOT NULL DEFAULT 0000,
  `serial` int(4) unsigned zerofill NOT NULL DEFAULT 0000,
  `serial_str` varchar(15) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL DEFAULT ''
) ENGINE=MyISAM DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

LOCK TABLES `serial` WRITE;
/*!40000 ALTER TABLE `serial` DISABLE KEYS */;
/*!40000 ALTER TABLE `serial` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

