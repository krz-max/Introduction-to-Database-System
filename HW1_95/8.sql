SELECT champion_name
FROM champ c
WHERE champion_id NOT IN (
    SELECT DISTINCT c.champion_id
    FROM champ c, teamban tb, match_info m
    WHERE tb.match_id = m.match_id
    AND tb.champion_id = c.champion_id
    AND SUBSTRING_INDEX(m.version, '.', 2) = '7.7'
)
ORDER BY champion_name;