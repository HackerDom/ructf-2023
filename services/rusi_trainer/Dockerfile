FROM node:21-alpine
WORKDIR /usr/rusi-trainer
COPY package.json .
RUN npm install
COPY . .
RUN npm run build