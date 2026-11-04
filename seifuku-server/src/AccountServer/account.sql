CREATE TABLE account (
 	id int(4) unsigned zerofill NOT NULL  auto_increment, 
	name char(32) binary NOT NULL  , 
	sex int(4) unsigned zerofill NOT NULL  , 
	age int(4) unsigned zerofill NOT NULL  , 
	phone char(16) binary NOT NULL  , 
	email char(128) binary NOT NULL  , 
	address char(128) binary NOT NULL  , 
	idnumber char(32) binary NOT NULL  , 
	password char(16) binary NOT NULL   ,
	PRIMARY KEY (id),
	KEY ix_name (name)	
) ENGINE=MyISAM;

CREATE TABLE fee (
  time_stamp char(14) binary NOT NULL default '',
  server_name char(15) binary NOT NULL default '',
  user_name char(15) binary NOT NULL default '',
  user_id int(4) unsigned zerofill NOT NULL default '0000',
  start_time char(19) binary NOT NULL default '',
  last_point char(19) binary NOT NULL default '',
  end_time char(19) binary NOT NULL default '',
  fee_name char(15) binary NOT NULL default '',
  fee_id int(4) unsigned zerofill NOT NULL default '0000',
  fee_type char(15) binary NOT NULL default '',
  point int(4) unsigned zerofill NOT NULL default '0000',
  end_type char(15) binary NOT NULL default '',
  ip_addr char(15) binary NOT NULL default ''
) ENGINE=MyISAM;

CREATE TABLE logon (
  time_stamp char(14) binary NOT NULL default '',
  account char(15) binary NOT NULL default '',
  account_id int(4) unsigned zerofill NOT NULL default '0000',
  fee_type int(4) unsigned zerofill NOT NULL default '0000',
  server char(15) binary NOT NULL default '',
  authen int(4) unsigned zerofill NOT NULL default '0000',
  client_ip char(15) binary NOT NULL default '',
  fee_account char(15) binary NOT NULL default '',
  notify char(15) binary NOT NULL default '',
  rejoin tinyint(1) unsigned zerofill NOT NULL default '0000'
) ENGINE=MyISAM;

CREATE TABLE serial (
  time_stamp char(14) binary NOT NULL default '',
  account_id int(4) unsigned zerofill NOT NULL default '0000',
  serial int(4) unsigned zerofill NOT NULL default '0000',
  serial_str varchar(15) binary NOT NULL default ''
) ENGINE=MyISAM;

CREATE TABLE point001 (
 	account_id int(4) unsigned zerofill NOT NULL  , 
	type int(4) unsigned zerofill NOT NULL  , 
	point int(4) unsigned zerofill NOT NULL  , 
	pointtime int(4) unsigned zerofill NOT NULL  , 
	online int(4) unsigned zerofill NOT NULL  , 
	licence int(4) unsigned NOT NULL  , 
	netbar_ip char(15)   , 
	ip_mask char(15)    ,
	PRIMARY KEY (account_id)	
) ENGINE=MyISAM;
