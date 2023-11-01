import * as grpc from 'grpc'

// eslint-disable-next-line no-undef
const {AutherClient} = require('./auth_grpc_web_pb')
// eslint-disable-next-line no-undef
const {LoginRequest, RegisterRequest} = require('./auth_pb')

const authClient = new AutherClient('localhost:42424')


export function loginUser(username, password) {
    const request = new LoginRequest()
    request.setUsername(username)
    request.setPassword(password)

    authClient.login(request, {}, (error, response) => {
        if (error) {
            console.log(error)
        } else {
            return response
        }
    })
}

export function registerUser(username, password, speciesType) {
    const request = new RegisterRequest()
    request.setUsername(username)
    request.setPassword(password)
    request.setSpeciesType(speciesType)

    authClient.register(request, {}, (error, response) => {
        if (error) {
            console.log(error)
        } else {
            return response
        }
    })
}

export function GetAncestorsList() {

}