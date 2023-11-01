FROM mcr.microsoft.com/dotnet/sdk:7.0 AS builder

COPY ./ ./

WORKDIR /src

RUN dotnet build ./API -c Release

FROM builder AS publish

RUN dotnet publish ./API -c Release -o /publish

FROM mcr.microsoft.com/dotnet/aspnet:7.0

COPY --from=publish /publish .

COPY entrypoint.sh .
RUN chmod +x ./entrypoint.sh

ENV ASPNETCORE_URLS=http://*:8000

ENTRYPOINT ./entrypoint.sh

