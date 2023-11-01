import { encode } from 'base69'

export function parseJwt(token) {
  const base64Url = token.split('.')[1]
  const base64 = base64Url.replace(/-/g, '+').replace(/_/g, '/')
  const jsonPayload = decodeURIComponent(
    window
      .atob(base64)
      .split('')
      .map(function (c) {
        return '%' + ('00' + c.charCodeAt(0).toString(16)).slice(-2)
      })
      .join('')
  )

  return JSON.parse(jsonPayload)
}
export async function registerMy(username, password) {
  const resp = await fetch('/auth/register/', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      Accept: 'application/json'
    },
    body: JSON.stringify({ username, password })
  })
  await validateResponse(resp)
}

export async function loginMy(username, password) {
  const resp = await fetch('/auth/login/', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      Accept: 'application/json'
    },
    body: JSON.stringify({ username, password })
  })
  await validateResponse(resp)
  const { accessToken } = await resp.json()
  return accessToken
}

export async function teachUdar(token, data) {
  const encoder = new TextEncoder()
  const resp = await fetch('/udar/', {
    method: 'PUT',
    headers: {
      'Content-Type': 'application/json',
      Accept: 'application/json',
      'x-access-token': token
    },
    body: JSON.stringify({ ...data, name: encode(encoder.encode(data.name)) })
  })
  await validateResponse(resp)
}

export async function getUdar(token, name) {
  const resp = await fetch(`/udar/${encodeURIComponent(name)}/`, {
    method: 'GET',
    headers: {
      Accept: 'application/json',
      'x-access-token': token
    }
  })
  await validateResponse(resp)
  return await resp.json()
}

export async function getUdars(token) {
  const resp = await fetch(`/udar/`, {
    method: 'GET',
    headers: {
      Accept: 'application/json',
      'x-access-token': token
    }
  })
  await validateResponse(resp)
  return await resp.json()
}

export async function drinkWater(from) {
  const resp = await fetch(`/voda/drink?lake=${encodeURIComponent(from)}`, {
    method: 'POST',
    headers: {
      Accept: 'application/json'
    }
  })
  await validateResponse(resp)
  const data = await resp.json()
  return data[0]['water']
}

export async function validateResponse(response) {
  if (!response.ok) {
    if (response.json) {
      const error = await response.json()
      if (error.message) {
        if (error.message === 'Validation Failed') throw new Error('Неправильный запрос смолвил')
        throw new Error(error.message)
      }
      if (error.password) {
        throw new Error(error.password.join(' '))
      }
      if (error.email) {
        throw new Error(error.email.join(' '))
      }
      if (error.username) {
        throw new Error(error.username.join(' '))
      }
      if (error.video) {
        throw new Error(error.password.join(' '))
      }
      if (error.text) {
        throw new Error(error.text.join(' '))
      }
      if (error.title) {
        throw new Error(error.title.join(' '))
      }
      throw new Error(error.detail)
    }
    throw new Error(response.statusText)
  }
}
