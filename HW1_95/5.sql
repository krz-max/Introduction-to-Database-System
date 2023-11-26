SELECT 
    CASE 
        WHEN matches_stats.win = 1 THEN 'win'
        ELSE 'lose'
    END AS win_lose,
    COUNT(*) AS cnt
FROM (
    SELECT 
        p.match_id, 
        s.win, 
        AVG(s.longesttimespentliving) AS avg_longest_time
    FROM participant p
    JOIN stat s ON p.player_id = s.player_id
    GROUP BY p.match_id, s.win
    HAVING AVG(s.longesttimespentliving) >= 1200
) AS matches_stats
GROUP BY win_lose
ORDER BY cnt;
