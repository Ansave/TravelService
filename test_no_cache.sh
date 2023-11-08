wrk -d 10 -t 5 -c 5 --latency -s ./get_no_cache.lua http://localhost:8080/
