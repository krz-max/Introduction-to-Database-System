WITH champion_counts AS (
    SELECT
        p.position,
        c.champion_name,
        RANK() OVER (PARTITION BY p.position ORDER BY COUNT(*) DESC) AS rk
    FROM champ c, participant p, match_info m
    WHERE c.champion_id = p.champion_id 
    AND p.match_id = m.match_id 
    AND m.duration BETWEEN 2400 AND 3000 
    AND p.position IN ('TOP', 'JUNGLE', 'MID', 'DUO_CARRY', 'DUO_SUPPORT')
    GROUP BY p.position, c.champion_name
)

SELECT position, champion_name
FROM champion_counts
WHERE rk = 1;
