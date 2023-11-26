SELECT c1.champion_name AS self_champ_name,
    SUM(s1.win) / COUNT(*) AS win_ratio,
    (SUM(s1.kills) + SUM(s1.assists)) / SUM(s1.deaths) AS self_kda,
    AVG(s1.goldearned) AS self_avg_gold,
    c2.champion_name AS enemy_champ_name,
    (SUM(s2.kills) + SUM(s2.assists)) / SUM(s2.deaths) AS enemy_kda,
    AVG(s2.goldearned) AS enemy_avg_gold,
    COUNT(*) AS battle_record
FROM participant p1
    JOIN champ c1 ON p1.champion_id = c1.champion_id
    JOIN participant p2 ON p1.match_id = p2.match_id AND p1.player_id != p2.player_id
    JOIN champ c2 ON p2.champion_id = c2.champion_id
    JOIN stat s1 ON s1.player_id = p1.player_id
    JOIN stat s2 ON s2.player_id = p2.player_id
WHERE c2.champion_name = 'Gragas'
    AND p1.position = 'TOP'
    AND p1.position = p2.position
GROUP BY c1.champion_name, c2.champion_name
HAVING COUNT(*) > 100
ORDER BY win_ratio DESC;