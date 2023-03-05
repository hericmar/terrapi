FROM rust:alpine

WORKDIR /usr/share/terrapi

RUN apk add --no-cache gcompat gcc musl-dev openssl-dev

COPY .cargo/     /usr/share/terrapi/.cargo
COPY etc/terrapi /etc/terrapi
COPY src/        /usr/share/terrapi/src
COPY Cargo.toml  /usr/share/terrapi/Cargo.toml

# RUN cargo build --package terrapi
