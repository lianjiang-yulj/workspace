use `flowable-test`;

select * from ACT_RU_VARIABLE;


SELECT concat('TRUNCATE TABLE ', table_name, ';')
FROM information_schema.tables
WHERE table_schema = 'flowable-test';


