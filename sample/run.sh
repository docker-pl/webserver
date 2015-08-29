docker run --name nginx -d \
  -v `pwd`/log:/var/log/nginx \
  -v `pwd`/upload:/var/upload \
  -p 8000:80 -p 8433:443 \
  -t pointlook/nginx

docker exec -it nginx bash
