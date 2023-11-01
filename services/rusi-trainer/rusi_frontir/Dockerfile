FROM node:21-alpine as build
LABEL stage=builder
WORKDIR /frontend
ENV PATH /frontend/node_modules/.bin:$PATH
COPY ./package.json /frontend/package.json
COPY ./package-lock.json /frontend/package-lock.json
RUN npm install
COPY ./ /frontend
RUN npm run build

# production environment
FROM nginx:1.16.0-alpine
COPY --from=build /frontend/dist /usr/share/nginx/html
RUN rm /etc/nginx/conf.d/default.conf
COPY nginx/nginx.conf /etc/nginx/conf.d/nginx.conf
EXPOSE 80
CMD ["nginx", "-g", "daemon off;"]