# build stage=
FROM node:16.13.2-alpine3.14 as build-stage
WORKDIR /app
COPY package*.json ./
RUN npm install
EXPOSE 8080
CMD ["npm", "run", "serve"] # For dev

# When we deploy app for production
# FROM nginx:stable-alpine as production-stage
# COPY --from=build-stage /app/dist /usr/share/nginx/html
# EXPOSE 80
# CMD ["nginx", "-g", "daemon off;"] # For production
