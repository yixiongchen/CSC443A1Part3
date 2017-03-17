CREATE VIEW outdegree as SELECT uid1 as uid, count(uid2) as count FROM R GROUP BY uid1;

CREATE VIEW indegree as SELECT uid2 as uid, count(uid1) as count FROM R GROUP BY uid2;

SELECT indegree.uid, indegree.count as indegree, outdegree.count as outdegree, (indegree.count - outdegree.count) as diff
   FROM indegree, outdegree
   WHERE indegree.uid = outdegree.uid 
   ORDER BY diff DESC
   LIMIT 10;