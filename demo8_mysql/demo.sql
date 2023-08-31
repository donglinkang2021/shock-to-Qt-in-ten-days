-- Active: 1692950880119@@127.0.0.1@3306@mydatabase

/* 查看用户 */
SELECT user,host FROM mysql.user;

/* 使用数据库mydatabase */
USE mydatabase;

/* 查看当前数据库 */
SELECT DATABASE();

/* 建立学生表 有id 姓名 电话 */
CREATE TABLE student(
    id INT UNSIGNED PRIMARY KEY,
    name VARCHAR(20) NOT NULL,
    birth DATE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/* 查看表 */
SHOW TABLES;

/* 查看表结构 */
DESC student;

/* 插入数据 */
INSERT INTO student(id,name,birth) VALUES(1,'张三','1999-01-01');

INSERT INTO student(id,name,birth) VALUES(2,'你好','2002-09-01');

/* 查看数据 */
SELECT * FROM student;

