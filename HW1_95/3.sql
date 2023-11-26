SELECT champion_name, COUNT(*) as cnt
FROM champ c
JOIN participant p ON c.champion_id = p.champion_id
WHERE p.position = 'JUNGLE'
GROUP BY champion_name
ORDER BY cnt DESC
LIMIT 3;
