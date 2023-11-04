package server

type Config struct {
	ServerHost string `json:"server_host"`
	ServerPort int    `json:"server_port"`
	RedisHost  string `json:"redis_host"`
	RedisPort  int    `json:"redis_port"`
}

type RegisterRequest struct {
	Username    string `json:"username"`
	PaymentInfo string `json:"payment_info"`
}

type RegisterResponse struct {
	Token        string `json:"token"`
	LuckyNumbers string `json:"lucky_numbers"`
}

type GetInfoRequest struct {
	Username string `json:"username"`
	Token    string `json:"token"`
}

type GetInfoResponse struct {
	PaymentInfo  string `json:"payment_info"`
	LuckyNumbers string `json:"lucky_numbers"`
}
