#include <iostream>
#include <curl/curl.h>
#include <string>
#include <chrono>
#include <random>
#include <conio.h>

// Функція для отримання поточного часу у відформатованому рядку
std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm gmTimeInfo;
    gmtime_s(&gmTimeInfo, &nowTime);

    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &gmTimeInfo); 
    return std::string(buffer);
}

// Функція для відправки запиту POST за допомогою libcurl
void sendPostRequest(const std::string& url, const std::string& jsonData, const std::string& authToken) {
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {

        // Встановлення заголовків для запиту
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + authToken).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        // Встановлення параметрів для запиту
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        // Виконання HTTP POST-запиту
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();
}

// Функція для моделювання даних і відправки POST-запитів
void simulateDataAndSend() {

    // Генерація даних
    std::string currentTime = getCurrentTime();
    std::random_device rd;
    std::mt19937 gen(rd());

    std::normal_distribution<double> temperatureDist(37.5, 1.5);
    int temperature = static_cast<int>(temperatureDist(gen));

    std::uniform_real_distribution<double> weightDist(717, 720);
    double weight = weightDist(gen);

    std::uniform_int_distribution<int> breathingRateDist(30, 45);
    int breathingRate = breathingRateDist(gen);

    std::uniform_int_distribution<int> pulseDist(60, 100);
    int pulse = pulseDist(gen);

    std::string activityTypes[] = { "Running", "Walking", "Eating", "Sleeping" };
    std::uniform_int_distribution<int> activityTypeDist(0, 3);
    std::string activityType = activityTypes[activityTypeDist(gen)];

    std::uniform_int_distribution<int> latitudeDist(-90, 90);
    int latitude = latitudeDist(gen);

    std::uniform_int_distribution<int> longitudeDist(-180, 180);
    int longitude = longitudeDist(gen);

    // Формування JSON-рядків для даних про активність та біометрію
    std::string activityData = R"({
        "AnimalID": 1,
        "ActivityType": ")" + activityType +
        R"(",
        "StartTime": ")" + currentTime +
        R"(",
        "EndTime": ")" + currentTime +
        R"(",
        "Latitude": )" + std::to_string(latitude) +
        R"(,
        "Longitude": )" + std::to_string(longitude) +
        "\n}"; 

    std::string biometricsData = R"({
        "AnimalID": 1,
        "Pulse": )" + std::to_string(pulse) +
        R"(,
        "Temperature": )" + std::to_string(temperature) +
        R"(,
        "Weight": )" + std::to_string(weight) +
        R"(,
        "BreathingRate": )" + std::to_string(breathingRate) +
        "\n}";  

    std::cout << "\n";
    std::cout << "Activity Data:\n" << activityData << std::endl;
    std::cout << "Biometrics Data:\n" << biometricsData << std::endl;
    std::cout << "\n";

    // URL-адреси ендпоінтів та токен автентифікації
    std::string activityEndpoint = "http://localhost:8000/api/farms/1/animals/1/activity";
    std::string biometricsEndpoint = "http://localhost:8000/api/farms/1/animals/1/biometrics";
    std::string authToken = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE3MDQwNjY1MTEsImlhdCI6MTcwNDAyMzMxMSwiVXNlcklEIjoxfQ.w8_7s3rK9mekgzvRif0kzaaGOJq_jrGhJrEnR4SyCPk"; 

    // Відправлення POST-запитів
    sendPostRequest(activityEndpoint, activityData, authToken);
    sendPostRequest(biometricsEndpoint, biometricsData, authToken);
}

int main() {
    bool stopSending = false;

    while (!stopSending) {
        simulateDataAndSend();
        Sleep(5000);
        if (_kbhit()) {
            char key = _getch();
            if (key == 27) { 
                stopSending = true;
                std::cout << "\nSending stopped by user.\n";
            }
        }
    }
    return 0;
}