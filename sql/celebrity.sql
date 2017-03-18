CREATE VIEW outdegree AS
SELECT uid1 AS uid, count(uid2) AS count
FROM R
GROUP BY uid1;

CREATE VIEW indegree AS
SELECT uid2 AS uid, count(uid1) AS count
FROM R
GROUP BY uid2;

SELECT indegree.uid, indegree.count AS indegree, outdegree.count AS outdegree, (indegree.count - outdegree.count) AS diff
FROM indegree, outdegree
WHERE indegree.uid = outdegree.uid 
ORDER BY diff DESC
LIMIT 10;