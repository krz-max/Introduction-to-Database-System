WITH champion_stats AS(
    SELECT p.position,
        c.champion_name,
        SUM(s.win) / COUNT(*) AS win_ratio,
        RANK() OVER (PARTITION BY p.position ORDER BY (SUM(s.win) / COUNT(*)) DESC) AS rk
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
    HAVING COUNT(*) > 300
)

SELECT position, champion_name, win_ratio
FROM champion_stats 
WHERE rk <= 5;
