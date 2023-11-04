package server

import (
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
)

var dbApi *DBApi

func handleRegister(w http.ResponseWriter, r *http.Request) {
	data, err := io.ReadAll(r.Body)
	if err != nil {
		w.WriteHeader(400)
		_, _ = w.Write([]byte("can not read data"))
		return
	}

	var registerRequest RegisterRequest
	if err := json.Unmarshal(data, &registerRequest); err != nil {
		w.WriteHeader(400)
		_, _ = w.Write([]byte("invalid json register body"))
		return
	}

	if !IsValid(registerRequest.Username) {
		w.WriteHeader(400)
		_, _ = w.Write([]byte("invalid username"))
		return
	}

	if err := dbApi.Register(registerRequest.Username, registerRequest.PaymentInfo); err != nil {
		w.WriteHeader(400)
		_, _ = w.Write([]byte("can not register user: " + err.Error()))
		return
	}

	token, luckyNumbers, err := dbApi.IssueData(registerRequest.Username)
	if err != nil {
		log.Println("can not register user: " + err.Error())
		return
	}

	response := RegisterResponse{
		Token:        token,
		LuckyNumbers: luckyNumbers,
	}

	rawResponse, err := json.Marshal(response)
	if err != nil {
		log.Println("can not marshal response: " + err.Error())
		return
	}

	if _, err := w.Write(rawResponse); err != nil {
		log.Println("can not write register response: " + err.Error())
		return
	}
}

func handleGetInfo(w http.ResponseWriter, r *http.Request) {
	data, err := io.ReadAll(r.Body)
	if err != nil {
		w.WriteHeader(400)
		_, _ = w.Write([]byte("can not read data"))
		return
	}

	var getInfoRequest GetInfoRequest
	if err := json.Unmarshal(data, &getInfoRequest); err != nil {
		w.WriteHeader(400)
		_, _ = w.Write([]byte("invalid json get info body"))
		return
	}

	if !dbApi.IsValidSecret(getInfoRequest.Username, getInfoRequest.Token) {
		w.WriteHeader(404)
		_, _ = w.Write([]byte("user does not exist"))
		return
	}

	info, err := dbApi.GetInfo(getInfoRequest.Username)
	if err != nil {
		log.Println("can not get user info")
		return
	}

	luckyNumbers, err := dbApi.GetLuckyNumbers(getInfoRequest.Username)
	if err != nil {
		log.Println("can not get user lucky numbers")
		return
	}

	response := GetInfoResponse{
		PaymentInfo:  info,
		LuckyNumbers: luckyNumbers,
	}

	rawResponse, err := json.Marshal(response)
	if err != nil {
		log.Println("can not marshal get info response: " + err.Error())
		return
	}

	if _, err := w.Write(rawResponse); err != nil {
		log.Println("can not write get info response: " + err.Error())
		return
	}
}

func RunServer(config *Config) {
	dbApi = NewDBApi(config)
	//res := dbApi.client.Set(context.Background(), "key", "hello", time.Hour)
	//fmt.Println(res.Err(), res.Err() == nil)
	//
	//value, err := dbApi.client.Get(context.Background(), "key").Result()
	//fmt.Println(value, err)

	http.HandleFunc("/register", handleRegister)
	http.HandleFunc("/get_info", handleGetInfo)
	log.Fatal(http.ListenAndServe(fmt.Sprintf("%s:%d", config.ServerHost, config.ServerPort), nil))
}
