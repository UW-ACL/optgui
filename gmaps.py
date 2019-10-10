import urllib

secret_key = "AIzaSyC8HvEZ7nhbklDrgQ3K-w_Lpw9ulGNSvcU"
gps = "47.653950,-122.306130"
request = "https://maps.googleapis.com/maps/api/staticmap?center=" + gps + "&zoom=12&size=400x400&key=" + secret_key

request = "https://maps.googleapis.com/maps/api/staticmap?center=47.653950,-122.306130&zoom=18&size=640x640&scale=2&maptype=satellite&key=AIzaSyC8HvEZ7nhbklDrgQ3K-w_Lpw9ulGNSvcU"
urllib.request.urlretrieve(request, 'image.png')
