SELECT COUNT(*) 
FROM 
(SELECT T.uid1, T.uid2
FROM 
	(SELECT * FROM R WHERE uid1 < uid2) as T, 
	(SELECT * FROM R WHERE uid2 < uid1) as S 
WHERE T.uid1 = S.uid2 AND T.uid2 = S.uid1);

