-- Find the top 10 most played champions in the top lane (position = 'TOP') with more than 1000 games played.
SELECT DISTINCT c.champion_id
FROM stat s
    JOIN participant p ON s.player_id = p.player_id
    JOIN champ c ON p.champion_id = c.champion_id
WHERE p.position = 'TOP'
GROUP BY c.champion_id
HAVING COUNT(*) > 1000
ORDER BY COUNT(*) DESC;
-- Calculate how many times each summoner skills were picked in the top lane when the champion was in the previous query.
SELECT c.champion_name AS self_champ_name,
    CASE
        WHEN p.ss1 < p.ss2 THEN p.ss1
        ELSE p.ss2
    END AS skill_one,
    CASE
        WHEN p.ss1 < p.ss2 THEN p.ss2
        ELSE p.ss1
    END AS skill_two,
    SUM(s.win) / COUNT(*) AS win_ratio,
    (SUM(s.kills) + SUM(s.assists)) / SUM(s.deaths) AS self_kda,
    COUNT(*) AS battle_record
FROM participant p
    JOIN champ c ON p.champion_id = c.champion_id
    JOIN stat s ON s.player_id = p.player_id
WHERE p.position = 'TOP'
    AND p.champion_id IN (
        SELECT DISTINCT c.champion_id
        FROM stat s
            JOIN participant p ON s.player_id = p.player_id
            JOIN champ c ON p.champion_id = c.champion_id
        WHERE p.position = 'TOP'
        GROUP BY c.champion_id
        HAVING COUNT(*) > 1000
    )
GROUP BY self_champ_name,
    skill_one,
    skill_two
HAVING COUNT(*) > 1000
ORDER BY (SUM(s.win) / COUNT(*)) DESC
LIMIT 30;

-- Calculate the win ratio of each summoner skills combination in the top lane regardless of the champion.
SELECT CASE
        WHEN p.ss1 < p.ss2 THEN p.ss1
        ELSE p.ss2
    END AS skill_one,
    CASE
        WHEN p.ss1 < p.ss2 THEN p.ss2
        ELSE p.ss1
    END AS skill_two,
    SUM(s.win) / COUNT(*) AS win_ratio,
    (SUM(s.kills) + SUM(s.assists)) / SUM(s.deaths) AS self_kda,
    COUNT(*) AS battle_record
FROM participant p
    JOIN champ c ON p.champion_id = c.champion_id
    JOIN stat s ON s.player_id = p.player_id
WHERE p.position = 'TOP'
    AND p.champion_id IN (
        SELECT DISTINCT c.champion_id
        FROM stat s
            JOIN participant p ON s.player_id = p.player_id
            JOIN champ c ON p.champion_id = c.champion_id
        WHERE p.position = 'TOP'
        GROUP BY c.champion_id
        HAVING COUNT(*) > 1000
    )
GROUP BY skill_one,
    skill_two
HAVING COUNT(*) > 1000
ORDER BY (SUM(s.win) / COUNT(*)) DESC;