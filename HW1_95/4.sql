SELECT match_id, 
       SEC_TO_TIME(duration) AS time
FROM match_info
ORDER BY duration DESC
LIMIT 5;
