WITH temp AS(
    SELECT SUBSTRING_INDEX(m.version, '.', 2) AS version,
        p1.match_id,
        c1.champion_name AS champ_c1,
        c2.champion_name AS champ_c2,
        s1.win AS win_s1,
        s2.win AS win_s2
    FROM participant p1
        JOIN champ c1 ON p1.champion_id = c1.champion_id
        JOIN participant p2 ON p1.match_id = p2.match_id
        JOIN champ c2 ON p2.champion_id = c2.champion_id
        JOIN stat s1 ON s1.player_id = p1.player_id
        JOIN stat s2 ON s2.player_id = p2.player_id
        JOIN match_info m ON m.match_id = p1.match_id
    WHERE c1.champion_name = 'Lee Sin'
        AND c2.champion_name = 'Teemo'
        AND s1.win = s2.win
)
SELECT version,
    SUM(win_s1) AS win_cnt,
    COUNT(*) - SUM(win_s1) AS lose_cnt,
    SUM(win_s1) / COUNT(*) AS win_ratio
FROM temp
GROUP BY version
ORDER BY version;