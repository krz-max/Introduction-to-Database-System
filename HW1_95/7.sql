WITH champion_stats AS(
    SELECT p.position,
        c.champion_name,
        ((SUM(s.kills) + SUM(s.assists)) / SUM(s.deaths)) AS kda,
        RANK() OVER (PARTITION BY p.position ORDER BY ((SUM(s.kills) + SUM(s.assists)) / SUM(s.deaths)) DESC) AS rk
    FROM champ c,
        participant p,
        stat s
    WHERE c.champion_id = p.champion_id
        AND p.player_id = s.player_id
        AND p.position IN (
            'TOP',
            'JUNGLE',
            'MID',
            'DUO_CARRY',
            'DUO_SUPPORT'
        )
    GROUP BY p.position,
        c.champion_name
    HAVING SUM(s.deaths) > 0
)

SELECT position, champion_name, kda
FROM champion_stats 
WHERE rk = 1;
